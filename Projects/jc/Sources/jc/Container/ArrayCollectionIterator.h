/*
	작성자 : 윤정도
	배열 계열 컨테이너의 값 이터레이터 (힙 할당 없음, 비가상)
	Const = true  : const 컬렉션 순회 + const T& 반환
	Const = false : 비 const 컬렉션 순회 + T& 반환
*/

#pragma once

#include "jc/Namespace.h"
#include "jc/TypeTraits.h"

NS_JC_BEGIN

// 전방 선언
template <typename, typename> class ArrayCollection;

template <typename T, typename TAllocator, bool Const>
class ArrayCollectionIterator
{
	using TArrayCollection		= ArrayCollection<T, TAllocator>;
	using TCollectionPtr		= Conditional_t<Const, const TArrayCollection*, TArrayCollection*>;
	using TValue				= Conditional_t<Const, const T, T>;

public:
	ArrayCollectionIterator(TCollectionPtr _pCollection, int _pos)
		: pCollection_(_pCollection)
		, pos_(_pos)
	{
	}

	bool HasNext() const
	{
		return IsValidIndex(pos_);
	}

	bool HasPrevious() const
	{
		return IsValidIndex(pos_ - 1);
	}

	TValue& Next()
	{
		return pCollection_->pArray_[pos_++];
	}

	TValue& Previous()
	{
		return pCollection_->pArray_[--pos_];
	}

	TValue& Current() const
	{
		return pCollection_->pArray_[pos_];
	}

	bool IsEnd() const
	{
		return HasNext() == false;
	}

	bool IsBegin() const
	{
		return HasPrevious() == false;
	}

	friend bool operator==(const ArrayCollectionIterator& _lhs, const ArrayCollectionIterator& _rhs)
	{
		return _lhs.pCollection_ == _rhs.pCollection_ && _lhs.pos_ == _rhs.pos_;
	}

	friend bool operator!=(const ArrayCollectionIterator& _lhs, const ArrayCollectionIterator& _rhs)
	{
		return !(_lhs == _rhs);
	}

protected:
	bool IsValidIndex(int _idx) const
	{
		return _idx >= 0 && _idx < pCollection_->Size();
	}

protected:
	TCollectionPtr pCollection_;
	int pos_;
};

NS_END
