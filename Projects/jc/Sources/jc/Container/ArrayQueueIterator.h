/*
	작성자 : 윤정도
*/

#pragma once

#include "jc/Namespace.h"
#include "jc/TypeTraits.h"

NS_JC_BEGIN

// 전방 선언

template <typename, typename>
class ArrayQueue;

template <typename T, typename TAllocator>
class ArrayQueueIterator
{
	using TArrayQueue = ArrayQueue<T, TAllocator>;

public:
	ArrayQueueIterator(TArrayQueue* _queue, int _pos)
		: pQueue_(_queue)
		, pos_(_pos)
	{
	}

	bool HasNext() const
	{
		return IsValidIndex(pos_);
	}

	bool HasPrevious() const
	{
		int pos = pos_ - 1;

		if (pos == -1)
		{
			pos = pQueue_->Capacity() - 1;
		}

		return IsValidIndex(pos);
	}

	T& Next()
	{
		TArrayQueue* pQueue = pQueue_;
		T& value = pQueue->pArray_[pos_++];

		if (pos_ == pQueue->Capacity())
		{
			pos_ = 0;
		}

		return value;
	}

	T& Previous()
	{
		TArrayQueue* pQueue = pQueue_;
		pos_ -= 1;

		if (pos_ == -1)
		{
			pos_ = pQueue->Capacity() - 1;
		}

		return pQueue->pArray_[pos_];
	}

	T& Current()
	{
		return pQueue_->pArray_[pos_];
	}

	bool IsEnd() const
	{
		return HasNext() == false;
	}

	bool IsBegin() const
	{
		return HasPrevious() == false;
	}

private:
	bool IsValidIndex(int _idx) const
	{
		TArrayQueue* pQueue = pQueue_;

		if (pQueue->IsEmpty())
		{
			return false;
		}

		if (pQueue->IsForwardedHead())
		{
			return (_idx >= pQueue->head_ && _idx < pQueue->Capacity()) ||
				   (_idx >= 0 && _idx < pQueue->tail_);
		}

		return _idx >= pQueue->head_ && _idx < pQueue->tail_;
	}

	TArrayQueue* pQueue_ = nullptr;
	int pos_ = 0;
};

NS_END
