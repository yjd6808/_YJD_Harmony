/*
 * 작성자: 윤정도
 * 생성일: 2/7/2023 11:08:00 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Limit.h>

#include <JCore/Container/Arrays.h>
#include <JCore/Container/HashMap.h>

#include <JCore/Pool/MemoryPoolAllocationAlgorithm.h>
#include <JCore/Pool/MemoryPoolAbstract.h>
#include <JCore/Pool/MemoryChuckQueue.h>


NS_JC_BEGIN


/*
 * =====================================================================
 *
 * 이진 탐색기반 메모리풀
 *
 * =====================================================================
 */
class BinarySearchMemoryPool :  public MemoryPoolAbstract
{
	using Type = BinarySearchMemoryPool;
public:
	BinarySearchMemoryPool(const HashMap<int, int>& allocationMap) : MemoryPoolAbstract(false) {
		BinarySearchMemoryPool::Initialize(allocationMap);
	}

	BinarySearchMemoryPool(bool skipInitialize) : MemoryPoolAbstract(skipInitialize) {
		BinarySearchMemoryPool::CreatePool();
	}

	BinarySearchMemoryPool(int slot, const String& name, bool skipInitialize = false) : MemoryPoolAbstract(slot, name, skipInitialize) {
		BinarySearchMemoryPool::CreatePool();
	}

	~BinarySearchMemoryPool() override {
		BinarySearchMemoryPool::Finalize();
	}

	template <int RequestSize>
	void* StaticPop()  {
		constexpr int iIndex = Detail::AllocationLengthMapConverter::ToIndex<RequestSize>();
		constexpr int iFitSize = Detail::AllocationLengthMapConverter::ToSize<iIndex>();

		bool bNewAlloc;
		void* pMemoryBlock = m_Pool[iIndex]->Pop(bNewAlloc);
		AddAllocated(iIndex, bNewAlloc);

		return pMemoryBlock;
	}

	void* DynamicPop(int requestSize, int& realAllocatedSize) override {
		int iIndex = Detail::AllocationLengthMapConverter::ToIndex(requestSize);
		int iFitSize = Detail::AllocationLengthMapConverter::ToSize(iIndex);

		realAllocatedSize = iFitSize;
		bool bNewAlloc;
		void* pMemoryBlock = m_Pool[iIndex]->Pop(bNewAlloc);
		AddAllocated(iIndex, bNewAlloc);
		return pMemoryBlock;
	}

	template <int PushSize>
	void StaticPush(void* memory) {
		// static_assert(Detail::AllocationLengthMapConverter::ValidateSize<PushSize>());
		int index = Detail::AllocationLengthMapConverter::ToIndex<PushSize>();
		AddDeallocated(index);
		m_Pool[index]->Push(memory);
	}

	void DynamicPush(void* memory, int returnSize) override {
		// DebugAssertMessage(Detail::AllocationLengthMapConverter::ValidateSize(returnSize), "뭐야! 사이즈가 안맞자나!");
		int index = Detail::AllocationLengthMapConverter::ToIndex(returnSize);
		AddDeallocated(index);
		m_Pool[index]->Push(memory);
	}


	void CreatePool() {
		for (int i = 0; i < Detail::MemoryBlockSizeMapSize_v; ++i) {
			int iChunkSize = Detail::AllocationLengthMapConverter::ToSize(i);
			m_Pool[i] = dbg_new MemoryChunckQueue(iChunkSize, 0);
		}
	}

	void Initialize(const HashMap<int, int>& allocationMap) override {
		DebugAssertMsg(m_bInitialized == false, "이미 풀이 초기화 되어 있습니다.");

		const_cast<HashMap<int, int>&>(allocationMap).Extension().ForEach([this](Pair<int, int>& count) {
			int iSize = count.Key;
			int iCount = count.Value;
			int iIndex = Detail::AllocationLengthMapConverter::ToIndex(iSize);
			DebugAssertMsg(Detail::AllocationLengthMapConverter::ValidateSize(iSize), "뭐야! 사이즈가 안맞자나!");

			if (m_Pool[iIndex]) {
				JCORE_DELETE_SAFE(m_Pool[iIndex]);
			}
			
			m_Pool[iIndex] = dbg_new MemoryChunckQueue(iSize, iCount);
			AddInitBlock(iIndex, iCount);
		});

		m_bInitialized = true;
	}

	// 반드시 프로그램 종료전 메모리풀을 더이상 사용하지 않을 때 호출하여 정리할 것
	void Finalize() override {
		DebugAssertMsg(HasUsingBlock() == false, "현재 사용중인 블록이 있습니다. !!!");

		for (int i = 0; i < Detail::MemoryBlockSizeMapSize_v; ++i) {
			JCORE_DELETE_SAFE(m_Pool[i]);
		}
	}

	int Algorithm() override { return eBinarySearch; }
	
private:
	MemoryChunckQueue* m_Pool[Detail::MemoryBlockSizeMapSize_v]{};
};

using BinaryMemoryPoolPtr = SharedPtr<BinarySearchMemoryPool>;

NS_JC_END