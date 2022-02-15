/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ListCollectionIterator.h>

namespace JCore {


// 전방 선언
					class  VoidOwner;
template <typename> struct ListNode;
template <typename> class  LinkedList;
template <typename T>
class LinkedListIterator : public ListCollectionIterator<T>
{
	using TListNode					= typename ListNode<T>;
	using TListCollectionIterator	= typename ListCollectionIterator<T>;
	using TLinkedList				= typename LinkedList<T>;
public:
	LinkedListIterator(VoidOwner& owner, TListNode* current) : TListCollectionIterator(owner, current) {}
	virtual ~LinkedListIterator() noexcept {}
public:
	virtual bool HasNext() const {
		return TListCollectionIterator::HasNext();
	}

	virtual bool HasPrevious() const {
		return TListCollectionIterator::HasPrevious();
	}

	virtual T& Next() {
		return TListCollectionIterator::Next();
	}

	virtual T& Previous() {
		return TListCollectionIterator::Previous();
	}
};

} // namespace JCore