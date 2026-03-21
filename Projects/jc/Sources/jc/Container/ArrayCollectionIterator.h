/*
	작성자 : 윤정도
*/

#pragma once

#include "jc/Container/Iterator.h"

NS_JC_BEGIN

// 전방 선언
class CVoidOwner;
template <typename, typename> class ArrayCollection;

template <typename T, typename TAllocator>
class JC_NOVTABLE ArrayCollectionIterator : public Iterator<T, TAllocator>
{
	using TIterator			= Iterator<T, TAllocator>;
	using TArrayCollection	= ArrayCollection<T, TAllocator>;

public:
	ArrayCollectionIterator(CVoidOwner& _owner, int _pos)
		: TIterator(_owner)
		, pos_(_pos)
	{
	}

	~ArrayCollectionIterator() noexcept override = 0;

public:
	bool HasNext() const override
	{
		if (!this->IsValid())
		{
			return false;
		}

		return IsValidIndex(pos_);
	}

	bool HasPrevious() const override
	{
		if (!this->IsValid())
		{
			return false;
		}

		return IsValidIndex(pos_ - 1);
	}

	T& Next() override
	{
		return CastArrayCollection()->pArray_[pos_++];
	}

	T& Previous() override
	{
		return CastArrayCollection()->pArray_[--pos_];
	}

	T& Current() override
	{
		return CastArrayCollection()->pArray_[pos_];
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
	virtual bool IsValidIndex(int _idx) const
	{
		return _idx >= 0 && _idx < CastArrayCollection()->Size();
	}

	TArrayCollection* CastArrayCollection() const
	{
		this->ThrowIfIteratorIsNotValid();
		return this->watcher_.Get<TArrayCollection*>();
	}

protected:
	int pos_;
};

template <typename T, typename TAllocator>
ArrayCollectionIterator<T, TAllocator>::~ArrayCollectionIterator() noexcept
{
	// UNUSED
}

NS_END
