/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ListCollectionIterator.h>

namespace JCore {


// 전방 선언
					class  VoidOwner;
template <typename> struct ListNode;
template <typename> class  ListQueue;
template <typename T>
class ListQueueIterator : public ListCollectionIterator<T>
{
	using TListNode					= typename ListNode<T>;
	using TListCollectionIterator	= typename ListCollectionIterator<T>;
	using TListQueue				= typename ListQueue<T>;
public:
	ListQueueIterator(VoidOwner& owner, TListNode* current) : TListCollectionIterator(owner, current) {}
	virtual ~ListQueueIterator() noexcept {}
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