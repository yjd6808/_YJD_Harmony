/*
	작성자 : 윤정도
*/

#pragma once

#include <jc/Container/ListCollectionIterator.h>

NS_JC_BEGIN


// 전방 선언
class  CVoidOwner;
template <typename, typename> class ListNode;
template <typename, typename> class LinkedList;
template <typename T, typename TAllocator>
class LinkedListIterator : public ListCollectionIterator<T, TAllocator>
{
	using TListNode				= ListNode<T, TAllocator>;
	using TListCollectionIterator	= ListCollectionIterator<T, TAllocator>;
	using TLinkedList			= LinkedList<T, TAllocator>;
public:
	LinkedListIterator(CVoidOwner& _owner, TListNode* _pCurrent)
		: TListCollectionIterator(_owner, _pCurrent)
	{
	}

	~LinkedListIterator() noexcept override
	{
	}

public:
	bool HasNext() const override
	{
		return TListCollectionIterator::HasNext();
	}

	bool HasPrevious() const override
	{
		return TListCollectionIterator::HasPrevious();
	}

	T& Next() override
	{
		return TListCollectionIterator::Next();
	}

	T& Previous() override
	{
		return TListCollectionIterator::Previous();
	}

	T& Current() override
	{
		return TListCollectionIterator::Current();
	}

	bool IsEnd() const override
	{
		return TListCollectionIterator::IsEnd();
	}

	bool IsBegin() const override
	{
		return TListCollectionIterator::IsBegin();
	}

	friend class TLinkedList;
};

NS_JC_END
