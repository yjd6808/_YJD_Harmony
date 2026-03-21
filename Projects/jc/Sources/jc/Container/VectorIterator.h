/*
	작성자 : 윤정도
*/

#pragma once

#include "jc/Container/ArrayCollectionIterator.h"

NS_JC_BEGIN

// 전방 선언
class CVoidOwner;
template <typename, typename> class Vector;

template <typename T, typename TAllocator>
class VectorIterator : public ArrayCollectionIterator<T, TAllocator>
{
	using TArrayCollectionIterator	= ArrayCollectionIterator<T, TAllocator>;
	using TVectorIterator			= VectorIterator<T, TAllocator>;
	using TVector					= Vector<T, TAllocator>;

public:
	VectorIterator(CVoidOwner& _owner, int _pos)
		: TArrayCollectionIterator(_owner, _pos)
	{
	}

	~VectorIterator() noexcept override = default;

public:
	bool HasNext() const override
	{
		return TArrayCollectionIterator::HasNext();
	}

	bool HasPrevious() const override
	{
		return TArrayCollectionIterator::HasPrevious();
	}

	T& Next() override
	{
		return TArrayCollectionIterator::Next();
	}

	T& Previous() override
	{
		return TArrayCollectionIterator::Previous();
	}

	T& Current() override
	{
		return TArrayCollectionIterator::Current();
	}

	bool IsEnd() const override
	{
		return TArrayCollectionIterator::IsEnd();
	}

	bool IsBegin() const override
	{
		return TArrayCollectionIterator::IsBegin();
	}

	friend bool operator==(const TVectorIterator& _lhs, const TVectorIterator& _rhs)
	{
		return _lhs.Watcher == _rhs.Watcher;
	}

	friend bool operator!=(const TVectorIterator& _lhs, const TVectorIterator& _rhs)
	{
		return _lhs.Watcher != _rhs.Watcher;
	}
};

NS_END
