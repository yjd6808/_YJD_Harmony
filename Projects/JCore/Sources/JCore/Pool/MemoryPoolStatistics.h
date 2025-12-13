/*
 * 작성자: 윤정도
 * 생성일: 12/14/2022 1:38:21 PM
 * =====================
 * 메모리풀 통계 기록
 * 실제 성능에 영향을 줄 수 있으므로 디버그시에만 사용토록 해야한다.
 * 아예 런타임중에 영향을 주지 않도록 구현할 것
 */


#pragma once

#include <JCore/Sync/ILock.h>
#include <JCore/Pool/MemoryPoolDetail.h>


NS_JC_BEGIN

class CMemoryPoolStatistics
{
public:
	Int64U GetInitAllocated()
	{
		return initAllocted_;
	}

	Int64U GetNewAllocated()
	{
		return newAlloctaed_;
	}

	Int64U GetTotalAllocated()
	{
		return initAllocted_ + newAlloctaed_;	// 메모리풀이 관리중인 메모리 크기
	}

	Int64U GetTotalUsed()
	{
		return totalUsed_;
	}

	Int64U GetTotalUsing()
	{
		return totalUsing_;
	}

	Int64U GetTotalReturned()
	{
		return totalReturned_;
	}

	int GetBlockTotalCounter(int _blockIndex)
	{
		return blockTotalCounter_[_blockIndex];
	}

	int GetBlockUsedCounter(int _blockIndex)
	{
		return blockUsedCounter_[_blockIndex];
	}

	int GetBlockNewAllocCounter(int _blockIndex)
	{
		return blockNewAllocCounter_[_blockIndex];
	}

	int GetBlockUsingCounter(int _blockIndex)
	{
		return blockUsingCounter_[_blockIndex];
	}

protected:
	void AddInitBlock(Int32 _blockIndex, Int32 _blockCount)
	{
		initAllocted_ += static_cast<Int64U>(Detail::AllocationLengthMapConverter::ToSize(_blockIndex)) * _blockCount;
		blockTotalCounter_[_blockIndex] += _blockCount;
	}

	void AddAllocated(Int32 _blockIndex, bool _createNew)
	{
		int size = Detail::AllocationLengthMapConverter::ToSize(_blockIndex);

		totalUsing_ += size;
		totalUsed_ += size;

		if (_createNew)
		{
			newAlloctaed_ += size;
			++blockTotalCounter_[_blockIndex];
			++blockNewAllocCounter_[_blockIndex];
		}

		++blockUsingCounter_[_blockIndex];
		++blockUsedCounter_[_blockIndex];
	}

	void AddDeallocated(Int32 _blockIndex)
	{
		const Int32 returnedSize = Detail::AllocationLengthMapConverter::ToSize(_blockIndex);

		--blockUsingCounter_[_blockIndex];
		totalUsing_ -= returnedSize;
		totalReturned_ += returnedSize;
	}

	void Reset()
	{
		initAllocted_ = 0;
		newAlloctaed_ = 0;
		totalUsed_ = 0;
		totalReturned_ = 0;
		totalUsing_ = 0;

		Arrays::Fill(blockTotalCounter_, 0);
		Arrays::Fill(blockUsedCounter_, 0);
		Arrays::Fill(blockNewAllocCounter_, 0);
		Arrays::Fill(blockUsingCounter_, 0);
	}

	// 현재 사용중인 블록이 있는지
	bool HasUsingBlock()
	{
		return Arrays::FindIf(blockUsingCounter_, [](const int& count) { return count > 0; }) != Detail::INVALID_INDEX;
	}

protected:
	AtomicInt64U initAllocted_{};      // MemoryPool::Initialize()시 제일 처음 할당된 메모리양
	AtomicInt64U newAlloctaed_{};      // MemoryPool::Initialize()때 할당된 메모리외에 추가로 새로 할당된 메모리양 (누적)
	AtomicInt64U totalUsed_{};         // 메모리풀을 얼마나 사용했는지
	AtomicInt64U totalReturned_{};     // 메모리풀로 얼마나 반환되었는지
	AtomicInt64U totalUsing_{};        // 현재 얼마나 사용중인지

	AtomicInt blockTotalCounter_[Detail::MemoryBlockSizeMapSize_v]{};     // 블록 종류별로 현재 몇개가 있는지 기록
	AtomicInt blockUsedCounter_[Detail::MemoryBlockSizeMapSize_v]{};      // 블록 종류별로 몇번 사용되었는지
	AtomicInt blockNewAllocCounter_[Detail::MemoryBlockSizeMapSize_v]{};  // 블록 종류별로 몇번 생성 할당되었는지 기록
	AtomicInt blockUsingCounter_[Detail::MemoryBlockSizeMapSize_v]{};     // 블록 종류별로 사용중인 블록 수 기록

	friend class MemoryPoolAbstract;
};

NS_JC_END
