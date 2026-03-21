/*
	작성자 : 윤정도
*/

#pragma once

#include "jc/Container/ArrayCollectionIterator.h"

NS_JC_BEGIN

// 전방 선언

template <typename, typename>
class ArrayQueue;
class CVoidOwner;

template <typename T, typename TAllocator>
class ArrayQueueIterator : public ArrayCollectionIterator<T, TAllocator>
{
	using TArrayCollectionIterator	= ArrayCollectionIterator<T, TAllocator>;
	using TArrayQueue				= ArrayQueue<T, TAllocator>;

public:
	ArrayQueueIterator(CVoidOwner& _owner, int _pos)
		: TArrayCollectionIterator(_owner, _pos)
	{
	}

	~ArrayQueueIterator() noexcept override = default;

public:
	bool HasNext() const override
	{
		if (!this->IsValid())
		{
			return false;
		}

		return IsValidIndex(this->pos_);
	}

	bool HasPrevious() const override
	{
		if (!this->IsValid())
		{
			return false;
		}

		int pos = this->pos_ - 1;

		if (pos == -1)
		{
			pos = CastArrayQueue()->Capacity() - 1;
		}

		return IsValidIndex(pos);
	}

	T& Next() override
	{
		TArrayQueue* pQueue = CastArrayQueue();
		T& value = pQueue->pArray_[this->pos_++];

		if (this->pos_ == pQueue->Capacity())
		{
			this->pos_ = 0;
		}

		return value;
	}

	T& Previous() override
	{
		TArrayQueue* pQueue = CastArrayQueue();
		this->pos_ -= 1;

		if (this->pos_ == -1)
		{
			this->pos_ = pQueue->Capacity() - 1;
		}

		return pQueue->pArray_[this->pos_];
	}

	T& Current() override
	{
		return CastArrayQueue()->pArray_[this->pos_];
	}

	bool IsEnd() const override
	{
		return HasNext() == false;
	}

	bool IsBegin() const override
	{
		return HasPrevious() == false;
	}

protected:
	/// <summary>
	/// [오버라이딩]
	/// - From CArrayCollectionIterator
	/// </summary>
	bool IsValidIndex(int _idx) const override
	{
		TArrayQueue* pQueue = CastArrayQueue();

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

	TArrayQueue* CastArrayQueue() const
	{
		this->ThrowIfIteratorIsNotValid();
		return this->watcher_.Get<TArrayQueue*>();
	}
};

NS_END
