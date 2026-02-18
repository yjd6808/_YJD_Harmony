/*
 * 작성자: 윤정도
 * 생성일: 2/7/2023 11:08:00 PM
 * =====================
 *
 */


#pragma once

#include <jc/Limit.h>

#include <jc/Container/Arrays.h>
#include <jc/Container/HashMap.h>

#include <jc/Pool/MemoryPoolAllocationAlgorithm.h>
#include <jc/Pool/MemoryPoolAbstract.h>
#include <jc/Pool/MemoryChuckQueue.h>


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
		constexpr int index = detail::AllocationLengthMapConverter::ToIndex<RequestSize>();
		constexpr int fitSize = detail::AllocationLengthMapConverter::ToSize<index>();

		bool newAlloc;
		void* pMemoryBlock = pPool_[index]->Pop(newAlloc);
#if _DEBUG
		AddAllocated(index, newAlloc);
#endif
		return pMemoryBlock;
	}

	void* DynamicPop(int _requestSize) override
	{
		int index = detail::AllocationLengthMapConverter::ToIndex(_requestSize);
		int fitSize = detail::AllocationLengthMapConverter::ToSize(index);

		bool newAlloc;
		void* pMemoryBlock = pPool_[index]->Pop(newAlloc);
#if _DEBUG
		AddAllocated(index, newAlloc);
#endif
		return pMemoryBlock;
	}

	void* DynamicPop(int _requestSize, int& _realAllocatedSize) override
	{
		int index = detail::AllocationLengthMapConverter::ToIndex(_requestSize);
		int fitSize = detail::AllocationLengthMapConverter::ToSize(index);

		_realAllocatedSize = fitSize;
		bool newAlloc;
		void* pMemoryBlock = pPool_[index]->Pop(newAlloc);
#if _DEBUG
		AddAllocated(index, newAlloc);
#endif
		return pMemoryBlock;
	}

	template <int PushSize>
	void StaticPush(void* _pMemory)
	{
		// static_assert(detail::AllocationLengthMapConverter::ValidateSize<PushSize>());
		int index = detail::AllocationLengthMapConverter::ToIndex<PushSize>();
#if _DEBUG
		AddDeallocated(index);
#endif
		pPool_[index]->Push(_pMemory);
	}

	void DynamicPush(void* _pMemory, int _returnSize) override
	{
		// DebugAssertMessage(detail::AllocationLengthMapConverter::ValidateSize(_returnSize), "뭐야! 사이즈가 안맞자나!");
		int index = detail::AllocationLengthMapConverter::ToIndex(_returnSize);
#if _DEBUG
		AddDeallocated(index);
#endif
		pPool_[index]->Push(_pMemory);
	}


	void CreatePool()
	{
		for (int i = 0; i < detail::MemoryBlockSizeMapSize_v; ++i)
		{
			int chunkSize = detail::AllocationLengthMapConverter::ToSize(i);
			pPool_[i] = dbg_new CMemoryChunckQueue(chunkSize, 0);
		}
	}

	void Initialize(const HashMap<int, int>& _allocationMap) override
	{
		jc_assert_msg(initialized_ == false, "이미 풀이 초기화 되어 있습니다.");

		const_cast<HashMap<int, int>&>(_allocationMap).Extension().ForEach([this](Pair<int, int>& count)
		{
			int size = count.key_;
			int countValue = count.value_;
			int index = detail::AllocationLengthMapConverter::ToIndex(size);
			jc_assert_msg(detail::AllocationLengthMapConverter::ValidateSize(size), "뭐야! 사이즈가 안맞자나!");

			if (pPool_[index])
			{
				JC_DELETE_SAFE(pPool_[index]);
			}

			pPool_[index] = dbg_new CMemoryChunckQueue(size, countValue);
			AddInitBlock(index, countValue);
		});

		initialized_ = true;
	}

	// 반드시 프로그램 종료전 메모리풀을 더이상 사용하지 않을 때 호출하여 정리할 것
	void Finalize() override
	{
		jc_assert_msg(HasUsingBlock() == false, "현재 사용중인 블록이 있습니다. !!!");

		for (int i = 0; i < detail::MemoryBlockSizeMapSize_v; ++i)
		{
			JC_DELETE_SAFE(pPool_[i]);
		}
	}

	int Algorithm() override
	{
		return eBinarySearch;
	}

private:
	CMemoryChunckQueue* pPool_[detail::MemoryBlockSizeMapSize_v]{};
};

using BinaryMemoryPoolPtr = SharedPtr<CBinarySearchMemoryPool>;

NS_END
