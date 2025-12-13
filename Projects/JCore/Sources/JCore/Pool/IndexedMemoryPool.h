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
 *	  단점은 타게터가 있기 떄문에 메모리풀이 너무 무겁다는 점이다.
 *	  남용하면 안됨.
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
	using MemoryChunkQueueTargetrList = Vector<CMemoryChunckQueue**>;
public:
	IndexedMemoryPool(const String& _name = nullptr)
		: MemoryPoolAbstract(_name)
	{
		IndexedMemoryPool::CreatePool();
		IndexedMemoryPool::CreateTargeters();
	}

	IndexedMemoryPool(const HashMap<int, int>& _allocationMap, const String& _name = nullptr)
		: MemoryPoolAbstract(_name)
	{
		IndexedMemoryPool::Initialize(_allocationMap);
		IndexedMemoryPool::CreatePool();
		IndexedMemoryPool::CreateTargeters();
	}

	~IndexedMemoryPool() override
	{
		IndexedMemoryPool::Finalize();
	}

	template <int RequestSize>
	void* StaticPop()
	{
		bool isNewAlloc;
		CMemoryChunckQueue* pChuckQueue = GetChunckQueue(RequestSize);

		if (pChuckQueue == nullptr)
			return nullptr;

		void* pMemoryBlock = pChuckQueue->Pop(isNewAlloc);
#ifdef DebugMode
		const int allocationIndex = Detail::AllocationLengthMapConverter::ToIndex(pChuckQueue->ChunkSize());
		AddAllocated(allocationIndex, isNewAlloc);
#endif
		return pMemoryBlock;
	}

	void* DynamicPop(int _requestSize) override
	{
		bool isNewAlloc;
		CMemoryChunckQueue* pChuckQueue = GetChunckQueue(_requestSize);
		if (pChuckQueue == nullptr)
			return nullptr;
		void* pMemoryBlock = pChuckQueue->Pop(isNewAlloc);
#ifdef DebugMode
		const int allocationIndex = Detail::AllocationLengthMapConverter::ToIndex(pChuckQueue->ChunkSize());
		AddAllocated(allocationIndex, isNewAlloc);
#endif
		return pMemoryBlock;
	}

	// TODO: 메모리할당 규칙이 Low와 High가 틀리기떄문에 벌어지는 현상이다.
	//       BinarySearch와 Indexed를 똑같이 사용하기 위해서는 "요청한" 값을 기록해놓고 "요청한" 값을 반납해야한다.
	//		 예를들어 617 Byte를 요청하면 1024바이트가 실제 할당되는데
	//       CBinarySearchMemoryPool의 경우 617바이트로 반환하든지, 1024바이트로 반환하든지 모두 올바른 반환이 이뤄지지만
	//       IndexedMemoryPool의 경우 617바이트로 반환해야지 올바른 반환이 이뤄진다.
	void* DynamicPop(int _requestSize, int& _realAllocatedSize) override
	{
		bool isNewAlloc;
		CMemoryChunckQueue* pChuckQueue = GetChunckQueue(_requestSize);
		if (pChuckQueue == nullptr)
			return nullptr;
		void* pMemoryBlock = pChuckQueue->Pop(isNewAlloc);
		_realAllocatedSize = pChuckQueue->ChunkSize();
#ifdef DebugMode
		const int allocationIndex = Detail::AllocationLengthMapConverter::ToIndex(_realAllocatedSize);
		AddAllocated(allocationIndex, isNewAlloc);
#endif
		return pMemoryBlock;
	}

	template <int PushSize>
	void StaticPush(void* _pMemory)
	{
		CMemoryChunckQueue* pChuckQueue = GetChunckQueue(PushSize);
		if (pChuckQueue == nullptr)
			return;
		const int chunkSize = pChuckQueue->ChunkSize();
		const int allocationIndex = Detail::AllocationLengthMapConverter::ToIndex(chunkSize);
		AddDeallocated(allocationIndex);
		pChuckQueue->Push(_pMemory);
	}

	void DynamicPush(void* _pMemory, int _returnSize) override
	{
		CMemoryChunckQueue* pChuckQueue = GetChunckQueue(_returnSize);
		if (pChuckQueue == nullptr)
			return;
		const int chunkSize = pChuckQueue->ChunkSize();
		const int index = Detail::AllocationLengthMapConverter::ToIndex(chunkSize);
		AddDeallocated(index);
		pChuckQueue->Push(_pMemory);
	}

	CMemoryChunckQueue* GetChunckQueue(int _size)
	{
		MemoryChunkQueueTargetrList* pTargeterList;
		int chunkQueueIndex = -1;

		if (_size > MaxAllocatableSize)
		{
			DebugAssertMsg(false, "풀인덱싱은 최대 %d 만큼만 할당가능합니다. (%d바이트)", MaxAllocatableSize, _size);
			return nullptr;
		}

		if (_size > LowBoundarySize)
		{
			chunkQueueIndex = _size / BoundarySizeMax;
			pTargeterList = poolTargeterHigh_Member;

			if (chunkQueueIndex < 0 || chunkQueueIndex >= HighTargeterListCapacity)
			{
				DebugAssertMsg(false, "올바르지 않은 청크큐 인덱스입니다. %d바이트 [%s]", _size, "하이");
				return nullptr;
			}
		}
		else
		{
			chunkQueueIndex = _size;
			pTargeterList = poolTargeterLow_Member;

			if (chunkQueueIndex < 0 || chunkQueueIndex >= LowTargeterListCapacity)
			{
				DebugAssertMsg(false, "올바르지 않은 청크큐 인덱스입니다. %d바이트 [%s]", _size, "하이");
				return nullptr;
			}
		}

		CMemoryChunckQueue* pChuckQueue = *pTargeterList->At(chunkQueueIndex);
		return pChuckQueue;
	}

	void CreatePool()
	{
		for (int boundaryIndex = 0; boundaryIndex <= HighBoundaryIndex; ++boundaryIndex)
		{
			int chunkSize = Detail::AllocationLengthMapConverter::ToSize(boundaryIndex);
			if (poolChunkQueueArray_Member[boundaryIndex] == nullptr)
				poolChunkQueueArray_Member[boundaryIndex] = dbg_new CMemoryChunckQueue(chunkSize, 0);
		}
	}

	void Initialize(const HashMap<int, int>& _allocationMap) override
	{
		DebugAssertMsg(initialized_ == false, "이미 풀이 초기화 되어 있습니다.");

		const_cast<HashMap<int, int>&>(_allocationMap).Extension().ForEach([this](Pair<int, int>& count)
		{
			const int blockSize = count.key_;
			const int blockCount = count.value_;
			const int allocationIndex = Detail::AllocationLengthMapConverter::ToIndex(blockSize);
			DebugAssertMsg(blockSize <= MaxAllocatableSize, "이 풀 인덱싱은 최대 %d 만큼만 할당가능합니다. (%d바이트 블록을 초기화하려함)", MaxAllocatableSize, blockSize);
			DebugAssertMsg(Detail::AllocationLengthMapConverter::ValidateSize(blockSize), "뭐야! 사이즈가 안맞자나!");
			if (poolChunkQueueArray_Member[allocationIndex])
				JCORE_DELETE_SAFE(poolChunkQueueArray_Member[allocationIndex]);

			poolChunkQueueArray_Member[allocationIndex] = dbg_new CMemoryChunckQueue(blockSize, blockCount);
			AddInitBlock(allocationIndex, blockCount);
		});

		initialized_ = true;
	}

	// 반드시 프로그램 종료전 메모리풀을 더이상 사용하지 않을 때 호출하여 정리할 것
	void Finalize() override
	{
		DebugAssertMsg(HasUsingBlock() == false, "현재 사용중인 블록이 있습니다. !!!");

		for (int boundaryIndex = 0; boundaryIndex <= HighBoundaryIndex; ++boundaryIndex)
		{
			JCORE_DELETE_SAFE(poolChunkQueueArray_Member[boundaryIndex]);
		}

		if (poolTargeterLow_Member != nullptr)
		{
			for (int targetIndex = 0; targetIndex < poolTargeterLow_Member->Size(); ++targetIndex)
			{
				poolTargeterLow_Member->At(targetIndex) = nullptr;
			}
			JCORE_DELETE_SAFE(poolTargeterLow_Member);
		}

		if (poolTargeterHigh_Member != nullptr)
		{
			for (int targetIndex = 0; targetIndex < poolTargeterHigh_Member->Size(); ++targetIndex)
			{
				poolTargeterHigh_Member->At(targetIndex) = nullptr;
			}
			JCORE_DELETE_SAFE(poolTargeterHigh_Member);
		}
	}

	int Algorithm() override { return eFullIndexing; }

	void CreateTargeters()
	{
		DebugAssertMsg(poolTargeterLow_Member == nullptr, "이미 Low 타게터 세팅이 되어있습니다.");
		DebugAssertMsg(poolTargeterHigh_Member == nullptr, "이미 High 타게터 세팅이 되어있습니다.");

		poolTargeterLow_Member = dbg_new MemoryChunkQueueTargetrList(LowTargeterListCapacity, nullptr);    // 513
		poolTargeterHigh_Member = dbg_new MemoryChunkQueueTargetrList(HighTargeterListCapacity, nullptr);  // 524
		int previousMaxSize = 0;

		// 1 ~ 512 바이트 (Low 타게터 할당)
		for (int boundaryIndex = 0; boundaryIndex <= LowBoundaryIndex; ++boundaryIndex)
		{
			int maxSize = 1 << boundaryIndex;

			// 1: 0 ~ 1 (0 제외)
			// 2: 2
			// 4: 3 ~ 4
			// 8: 5 ~ 8
			for (int sizeIndex = previousMaxSize + 1; sizeIndex <= maxSize; ++sizeIndex)
			{
				poolTargeterLow_Member->At(sizeIndex) = &poolChunkQueueArray_Member[boundaryIndex];
			}

			previousMaxSize = maxSize;
		}

		// 513 ~ 524288 바이트 (High 타게터 할당)
		previousMaxSize = 0;
		for (int boundaryIndex = LowBoundaryIndex + 1; boundaryIndex <= HighBoundaryIndex; ++boundaryIndex)
		{
			int maxSize = (1 << boundaryIndex) / BoundarySizeMax;

			// maxSize    1000으로 나눴을때 몫
			// 1024  : -> 0
			// 2048  : -> 1
			// 4096  : -> 2 ~ 3
			// 8192  : -> 4 ~ 7
			// 16384 : -> 8 ~ 15
			// 
			// 4001바이트를 주문하면 8192바이트를 주게되네
			// 살짝 손해가 있긴하다.

			for (int sizeIndex = previousMaxSize; sizeIndex <= maxSize - 1; ++sizeIndex)
			{
				poolTargeterHigh_Member->At(sizeIndex) = &poolChunkQueueArray_Member[boundaryIndex];
			}

			previousMaxSize = maxSize;
		}
	}

public:
	static constexpr int LowBoundaryIndex = 9;
	static constexpr int HighBoundaryIndex = 19;

	static constexpr int LowBoundarySize = 1 << LowBoundaryIndex;		// 512		3자리 중 제일 큰 수
	static constexpr int HighBoundarySize = 1 << HighBoundaryIndex;		// 524'288	6자리 중 제일 큰 수
	static constexpr int BoundarySizeMax = 1000;							// 3자리수 최대 + 1

	static constexpr int LowTargeterListCapacity = LowBoundarySize + 1;	// 513
	static constexpr int HighTargeterListCapacity = HighBoundarySize / BoundarySizeMax;	// 524

	static constexpr int MaxAllocatableSize = (HighBoundarySize / 1000 - 1) * 1000;	// 최대 할당 가능한 메모리 (523'000)

private:
	CMemoryChunckQueue* poolChunkQueueArray_Member[Detail::MemoryBlockSizeMapSize_v]{};
	MemoryChunkQueueTargetrList* poolTargeterLow_Member{};
	MemoryChunkQueueTargetrList* poolTargeterHigh_Member{};
};


using IndexedMemoryPoolPtr = SharedPtr<IndexedMemoryPool>;

NS_JC_END
