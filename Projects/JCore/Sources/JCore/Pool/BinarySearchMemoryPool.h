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
class CBinarySearchMemoryPool : public MemoryPoolAbstract
{
	using Type = CBinarySearchMemoryPool;

public:
	CBinarySearchMemoryPool(const String& _name = nullptr)
		: MemoryPoolAbstract(_name)
	{
		CBinarySearchMemoryPool::CreatePool();
	}

	CBinarySearchMemoryPool(const HashMap<int, int>& _allocationMap, const String& _name = nullptr)
		: MemoryPoolAbstract(_name)
	{
		CBinarySearchMemoryPool::Initialize(_allocationMap);
	}

	~CBinarySearchMemoryPool() override
	{
		CBinarySearchMemoryPool::Finalize();
	}

	template <int RequestSize>
	void* StaticPop()
	{
		constexpr int index = Detail::AllocationLengthMapConverter::ToIndex<RequestSize>();
		constexpr int fitSize = Detail::AllocationLengthMapConverter::ToSize<index>();

		bool newAlloc;
		void* pMemoryBlock = pPool_[index]->Pop(newAlloc);
#if DebugMode
		AddAllocated(index, newAlloc);
#endif
		return pMemoryBlock;
	}

	void* DynamicPop(int _requestSize) override
	{
		int index = Detail::AllocationLengthMapConverter::ToIndex(_requestSize);
		int fitSize = Detail::AllocationLengthMapConverter::ToSize(index);

		bool newAlloc;
		void* pMemoryBlock = pPool_[index]->Pop(newAlloc);
#if DebugMode
		AddAllocated(index, newAlloc);
#endif
		return pMemoryBlock;
	}

	void* DynamicPop(int _requestSize, int& _realAllocatedSize) override
	{
		int index = Detail::AllocationLengthMapConverter::ToIndex(_requestSize);
		int fitSize = Detail::AllocationLengthMapConverter::ToSize(index);

		_realAllocatedSize = fitSize;
		bool newAlloc;
		void* pMemoryBlock = pPool_[index]->Pop(newAlloc);
#if DebugMode
		AddAllocated(index, newAlloc);
#endif
		return pMemoryBlock;
	}

	template <int PushSize>
	void StaticPush(void* _pMemory)
	{
		// static_assert(Detail::AllocationLengthMapConverter::ValidateSize<PushSize>());
		int index = Detail::AllocationLengthMapConverter::ToIndex<PushSize>();
#if DebugMode
		AddDeallocated(index);
#endif
		pPool_[index]->Push(_pMemory);
	}

	void DynamicPush(void* _pMemory, int _returnSize) override
	{
		// DebugAssertMessage(Detail::AllocationLengthMapConverter::ValidateSize(_returnSize), "뭐야! 사이즈가 안맞자나!");
		int index = Detail::AllocationLengthMapConverter::ToIndex(_returnSize);
#if DebugMode
		AddDeallocated(index);
#endif
		pPool_[index]->Push(_pMemory);
	}


	void CreatePool()
	{
		for (int i = 0; i < Detail::MemoryBlockSizeMapSize_v; ++i)
		{
			int chunkSize = Detail::AllocationLengthMapConverter::ToSize(i);
			pPool_[i] = dbg_new CMemoryChunckQueue(chunkSize, 0);
		}
	}

	void Initialize(const HashMap<int, int>& _allocationMap) override
	{
		DebugAssertMsg(initialized_ == false, "이미 풀이 초기화 되어 있습니다.");

		const_cast<HashMap<int, int>&>(_allocationMap).Extension().ForEach([this](Pair<int, int>& count)
		{
			int size = count.key_;
			int countValue = count.value_;
			int index = Detail::AllocationLengthMapConverter::ToIndex(size);
			DebugAssertMsg(Detail::AllocationLengthMapConverter::ValidateSize(size), "뭐야! 사이즈가 안맞자나!");

			if (pPool_[index])
			{
				JCORE_DELETE_SAFE(pPool_[index]);
			}

			pPool_[index] = dbg_new CMemoryChunckQueue(size, countValue);
			AddInitBlock(index, countValue);
		});

		initialized_ = true;
	}

	// 반드시 프로그램 종료전 메모리풀을 더이상 사용하지 않을 때 호출하여 정리할 것
	void Finalize() override
	{
		DebugAssertMsg(HasUsingBlock() == false, "현재 사용중인 블록이 있습니다. !!!");

		for (int i = 0; i < Detail::MemoryBlockSizeMapSize_v; ++i)
		{
			JCORE_DELETE_SAFE(pPool_[i]);
		}
	}

	int Algorithm() override
	{
		return eBinarySearch;
	}

private:
	CMemoryChunckQueue* pPool_[Detail::MemoryBlockSizeMapSize_v]{};
};

using BinaryMemoryPoolPtr = SharedPtr<CBinarySearchMemoryPool>;

NS_JC_END
