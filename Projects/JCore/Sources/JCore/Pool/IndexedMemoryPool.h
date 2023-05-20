/*
 * 작성자: 윤정도
 * 생성일: 02/06/2022
 * =====================
 * 우선 락프리 컨테이너를 구현하기전에는 뮤텍스로 원자적 수행을 보장토록 해준다.
 * 메모리풀 로직을 어떻게 구현할까?
 *
 * 1. 53바이트를 요청하면 64바이트를 줘야하는데.
 *    이걸 엄청 빠르게 런타임에 계산하는 방법이 1 ~ 1 << 29사이를 이분탐색으로 확인하는 방법
 *    ==> BinarySearch으로 명명함
 *
 * 2. 혹은 보통 메모리 할당요청하는게 보통 1만 바이트이하니까
 *	  m_Pool[0 ~ 1]  -> &pool[0] // 1바이트 풀
 *	  m_Pool[2]		 -> &pool[1] // 2바이트 풀
 *	  m_Pool[3 ~ 4]  -> &pool[2] // 4바이트 풀
 *	  m_Pool[5 ~ 7]  -> &pool[3] // 8바이트 풀
 *	  m_Pool[8 ~ 15] -> &pool[4] // 16바이트 풀
 *	  이런식으로 O(1)의 속도로 Push/Pop 가능하게 만들어주면 될 듯?
 *
 *	  아래 내가 구현한 방식은 4바이트 포인터 메모리 낭비를 좀 줄이기 위해
 *	  바운더리를 정했다.
 *    예를들어 바운더리가 3이면 (2의 배수중 3자리 최대: 512를 기점으로)
 *
 *	  512바이트 이하는 사이즈 그대로 인덱싱
 *	  512바이트 초과는 1000으로 나눠서 인덱싱 하도록 함.
 *
 * 3. 고정 사이즈를 할당해주는 효율적인 방법은 없을까?
 *    이건 나중에 고민하는걸로
 *
 *
 * ======================================================================
 */


#pragma once

#include <JCore/Limit.h>

#include <JCore/Container/Arrays.h>
#include <JCore/Container/Vector.h>
#include <JCore/Container/HashMap.h>

#include <JCore/Pool/MemoryPoolAllocationAlgorithm.h>
#include <JCore/Pool/MemoryPoolAbstract.h>
#include <JCore/Pool/MemoryChuckQueue.h>

NS_JC_BEGIN


class IndexedMemoryPool : public MemoryPoolAbstract
{
	using MemoryChunkQueueTargetrList = JCore::Vector<MemoryChunckQueue**>;
public:
	IndexedMemoryPool(const HashMap<int, int>& allocationMap) : MemoryPoolAbstract(false) {
		IndexedMemoryPool::Initialize(allocationMap);
		IndexedMemoryPool::CreatePool();
		IndexedMemoryPool::CreateTargeters();
	}

	IndexedMemoryPool(bool skipInitialize) : MemoryPoolAbstract(skipInitialize) {
		IndexedMemoryPool::CreatePool();
		IndexedMemoryPool::CreateTargeters();
	}

	IndexedMemoryPool(int slot, const String& name, bool skipInitialize = false) : MemoryPoolAbstract(slot, name, skipInitialize) {
		IndexedMemoryPool::CreatePool();
		IndexedMemoryPool::CreateTargeters();
	}

	~IndexedMemoryPool() override {
		IndexedMemoryPool::Finalize();
	}

	template <int RequestSize>
	void* StaticPop() {
		constexpr int iIndex = Detail::AllocationLengthMapConverter::ToIndex<RequestSize>();

		bool bNewAlloc;
		MemoryChunckQueue* pChuckQueue = GetChunckQueue(RequestSize);

		if (pChuckQueue == nullptr) {
			return nullptr;
		}

		void* pMemoryBlock = pChuckQueue->Pop(bNewAlloc);
#ifdef DebugMode
		AddAllocated(iIndex, bNewAlloc);
#endif
		return pMemoryBlock;
	}


	// TODO: 메모리할당 규칙이 Low와 High가 틀리기떄문에 벌어지는 현상이다.
	//       BinarySearch와 Indexed를 똑같이 사용하기 위해서는 "요청한" 값을 기록해놓고 "요청한" 값을 반납해야한다.
	//		 좀더 유연한 구조로 개선 필요. ㅠㅠ
	void* DynamicPop(int requestSize, int& realAllocatedSize) override {

		bool bNewAlloc;
		MemoryChunckQueue* pChuckQueue = GetChunckQueue(requestSize);
		if (pChuckQueue == nullptr) return nullptr;
		void* pMemoryBlock = pChuckQueue->Pop(bNewAlloc);
		realAllocatedSize = pChuckQueue->ChunkSize();
#ifdef DebugMode
		const int iIndex = Detail::AllocationLengthMapConverter::ToIndex(pChuckQueue->ChunkSize());
		AddAllocated(iIndex, bNewAlloc);
#endif
		return pMemoryBlock;
	}

	template <int PushSize>
	void StaticPush(void* memory) {
		MemoryChunckQueue* pChuckQueue = GetChunckQueue(PushSize);
		if (pChuckQueue == nullptr) return;
		const int iIndex = Detail::AllocationLengthMapConverter::ToIndex(pChuckQueue->ChunkSize());
		AddDeallocated(iIndex);
		pChuckQueue->Push(memory);
	}

	void DynamicPush(void* memory, int returnSize) override {
		MemoryChunckQueue* pChuckQueue = GetChunckQueue(returnSize);
		if (pChuckQueue == nullptr) return;
		const int index = Detail::AllocationLengthMapConverter::ToIndex(pChuckQueue->ChunkSize());
		AddDeallocated(index);
		pChuckQueue->Push(memory);
	}

	MemoryChunckQueue* GetChunckQueue(int size) {
		MemoryChunkQueueTargetrList* pTargeterList;
		int iChunkQueueIndex = -1;

		if (size > MaxAllocatableSize) {
			DebugAssertMsg(false, "풀인덱싱은 최대 %d 만큼만 할당가능합니다. (%d바이트)", MaxAllocatableSize, size);
			return nullptr;
		}

		if (size > LowBoundarySize) {
			iChunkQueueIndex = size / BoundarySizeMax;
			pTargeterList = m_PoolTargeterHigh;

			if (iChunkQueueIndex <= 0 || iChunkQueueIndex >= HighTargeterListCapacity) {
				DebugAssertMsg(false, "올바르지 않은 청크큐 인덱스입니다. %d바이트 [%s]", size, "하이");
				return nullptr;
			}
		}
		else {
			iChunkQueueIndex = size;
			pTargeterList = m_PoolTargeterLow;

			if (iChunkQueueIndex < 0 || iChunkQueueIndex >= LowTargeterListCapacity) {
				DebugAssertMsg(false, "올바르지 않은 청크큐 인덱스입니다. %d바이트 [%s]", size, "하이");
				return nullptr;
			}
		}

		MemoryChunckQueue* pChuckQueue = *pTargeterList->At(iChunkQueueIndex);
		return pChuckQueue;
	}

	void CreatePool() {
		for (int i = 0; i <= HighBoundaryIndex; ++i) {
			int iChunkSize = Detail::AllocationLengthMapConverter::ToSize(i);
			if (m_Pool[i] == nullptr) m_Pool[i] = dbg_new MemoryChunckQueue(iChunkSize, 0);
		}
	}

	void Initialize(const HashMap<int, int>& allocationMap) override {
		DebugAssertMsg(m_bInitialized == false, "이미 풀이 초기화 되어 있습니다.");

		const_cast<HashMap<int, int>&>(allocationMap).Extension().ForEach([this](Pair<int, int>& count) {
			const int iSize = count.Key;
			const int iCount = count.Value;
			const int iIndex = Detail::AllocationLengthMapConverter::ToIndex(iSize);
			DebugAssertMsg(iSize <= MaxAllocatableSize, "이 풀 인덱싱은 최대 %d 만큼만 할당가능합니다. (%d바이트 블록을 초기화하려함)", MaxAllocatableSize, iSize);
			DebugAssertMsg(Detail::AllocationLengthMapConverter::ValidateSize(iSize), "뭐야! 사이즈가 안맞자나!");
			if (m_Pool[iIndex])
				JCORE_DELETE_SAFE(m_Pool[iIndex]);

			m_Pool[iIndex] = dbg_new MemoryChunckQueue(iSize, iCount);
			AddInitBlock(iIndex, iCount);
		});

		m_bInitialized = true;
	}

	// 반드시 프로그램 종료전 메모리풀을 더이상 사용하지 않을 때 호출하여 정리할 것
	void Finalize() override {
		DebugAssertMsg(HasUsingBlock() == false, "현재 사용중인 블록이 있습니다. !!!");

		for (int i = 0; i <= HighBoundaryIndex; ++i) {
			JCORE_DELETE_SAFE(m_Pool[i]);
		}

		if (m_PoolTargeterLow != nullptr) {
			for (int i = 0; i < m_PoolTargeterLow->Size(); ++i) {
				m_PoolTargeterLow->At(i) = nullptr;
			}
			JCORE_DELETE_SAFE(m_PoolTargeterLow);
		}

		if (m_PoolTargeterHigh != nullptr) {
			for (int i = 0; i < m_PoolTargeterHigh->Size(); ++i) {
				m_PoolTargeterHigh->At(i) = nullptr;
			}
			JCORE_DELETE_SAFE(m_PoolTargeterHigh);
		}
	}

	int Algorithm() override { return eFullIndexing; }


	void CreateTargeters() {

		DebugAssertMsg(m_PoolTargeterLow == nullptr, "이미 Low 타게터 세팅이 되어있습니다.");
		DebugAssertMsg(m_PoolTargeterHigh == nullptr, "이미 High 타게터 세팅이 되어있습니다.");

		m_PoolTargeterLow = dbg_new MemoryChunkQueueTargetrList(LowTargeterListCapacity, nullptr);	// 513
		m_PoolTargeterHigh = dbg_new MemoryChunkQueueTargetrList(HighTargeterListCapacity, nullptr);	// 524
		int iBeforeMax = 0;

		// 1 ~ 512 바이트 (Low 타게터 할당)
		for (int i = 0; i <= LowBoundaryIndex; ++i) {
			
			int iMaxSize = 1 << i;

			// 1: 0 ~ 1 (0 제외)
			// 2: 2
			// 4: 3 ~ 4
			// 8: 5 ~ 8
			for (int iSize = iBeforeMax + 1; iSize <= iMaxSize; ++iSize) {
				m_PoolTargeterLow->At(iSize) = &m_Pool[i];
			}

			iBeforeMax = iMaxSize;
		}


		// 513 ~ 524288 바이트 (High 타게터 할당)
		iBeforeMax = 0;
		for (int i = LowBoundaryIndex + 1; i <= HighBoundaryIndex; ++i) {
			int iMaxSize = (1 << i) / BoundarySizeMax;


			// iMaxSize    1000으로 나눴을때 몫
			// 1024  : -> 0
			// 2048  : -> 1
			// 4096  : -> 2 ~ 3
			// 8192  : -> 4 ~ 7
			// 16384 : -> 8 ~ 15
			// 
			// 4001바이트를 주문하면 8192바이트를 주게되네
			// 살짝 손해가 있긴하다.

			for (int iSize = iBeforeMax; iSize <= iMaxSize - 1; ++iSize) {
				m_PoolTargeterHigh->At(iSize) = &m_Pool[i];
			}

			
			iBeforeMax = iMaxSize;
		}
	}



public:
	static constexpr int LowBoundaryIndex = 9;
	static constexpr int HighBoundaryIndex = 19;

	static constexpr int LowBoundarySize = 1 << LowBoundaryIndex;		// 512		3자리 중 제일 큰 수
	static constexpr int HighBoundarySize = 1 << HighBoundaryIndex;		// 524'288	6자리 중 제일 큰 수
	static constexpr int BoundarySizeMax = 1000;						// 3자리수 최대 + 1

	static constexpr int LowTargeterListCapacity = LowBoundarySize + 1;	// 513
	static constexpr int HighTargeterListCapacity = HighBoundarySize / BoundarySizeMax;	// 524

	static constexpr int MaxAllocatableSize = (HighBoundarySize / 1000 - 1) * 1000;	// 최대 할당 가능한 메모리 (523'000)
private:
	MemoryChunckQueue* m_Pool[Detail::MemoryBlockSizeMapSize_v]{};
	MemoryChunkQueueTargetrList* m_PoolTargeterLow{};
	MemoryChunkQueueTargetrList* m_PoolTargeterHigh{};
};


using IndexedMemoryPoolPtr = SharedPtr<IndexedMemoryPool>;

NS_JC_END

