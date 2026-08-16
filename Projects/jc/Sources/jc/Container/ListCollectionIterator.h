/*
	작성자 : 윤정도
	리스트 계열 컨테이너의 값 이터레이터 (힙 할당 없음, 비가상)
	Const = true  : const 컬렉션 순회 + const T& 반환
	Const = false : 비 const 컬렉션 순회 + T& 반환
*/

#pragma once

#include "jc/Namespace.h"
#include "jc/TypeTraits.h"
#include "jc/Exception.h"

#include "jc/Container/ListNode.h"

NS_JC_BEGIN

// 전방 선언
template <typename, typename> class ListCollection;

template <typename T, typename TAllocator, bool Const>
class ListCollectionIterator
{
	using TListCollection		= ListCollection<T, TAllocator>;
	using TListNode				= ListNode<T, TAllocator>;
	using TListPtr				= Conditional_t<Const, const TListCollection*, TListCollection*>;
	using TValue				= Conditional_t<Const, const T, T>;

public:
	ListCollectionIterator(TListPtr _pList, TListNode* _pCurrent)
		: pCollection_(_pList)
		, pCurrent_(_pCurrent)
	{
	}

	bool HasNext() const
	{
		return pCurrent_ != nullptr;
	}

	bool HasPrevious() const
	{
		return pCurrent_ != nullptr;
	}

	TValue& Next()
	{
		// 반복자가 꼬리까지 도달했는데 데이터를 가져올려고 시도하는 경우
		if (pCurrent_ == nullptr)
		{
			throw InvalidOperationException("데이터가 없습니다.");
		}

		TValue& value = pCurrent_->value_;
		pCurrent_ = pCurrent_->pNext_;
		return value;
	}

	TValue& Previous()
	{
		if (pCurrent_ == nullptr)
		{
			throw InvalidOperationException("데이터가 없습니다.");
		}

		TValue& value = pCurrent_->value_;
		pCurrent_ = pCurrent_->pPrevious_;
		return value;
	}

	TValue& Current() const
	{
		return pCurrent_->value_;
	}

	bool IsEnd() const
	{
		return pCurrent_ == nullptr;
	}

	bool IsBegin() const
	{
		return pCurrent_ == pCollection_->pHead_;
	}

	friend bool operator==(const ListCollectionIterator& _lhs, const ListCollectionIterator& _rhs)
	{
		return _lhs.pCollection_ == _rhs.pCollection_ && _lhs.pCurrent_ == _rhs.pCurrent_;
	}

	friend bool operator!=(const ListCollectionIterator& _lhs, const ListCollectionIterator& _rhs)
	{
		return !(_lhs == _rhs);
	}

protected:
	TListPtr pCollection_;
	TListNode* pCurrent_;

	friend class TListCollection;
};


NS_END
