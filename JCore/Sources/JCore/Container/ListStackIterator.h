/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ListCollectionIterator.h>

namespace JCore {


// 전방 선언
					class  VoidOwner;
template <typename> struct ListNode;
template <typename> class  ListStack;
template <typename T>
class ListStackIterator : public ListCollectionIterator<T>
{
	using TListNode					= typename ListNode<T>;
	using TListCollectionIterator	= typename ListCollectionIterator<T>;
	using TListStack				= typename ListStack<T>;
public:
	ListStackIterator(VoidOwner& owner, TListNode* current) : TListCollectionIterator(owner, current) {}
	virtual ~ListStackIterator() noexcept {}
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

	virtual bool IsEnd() const {
		return TListCollectionIterator::IsEnd();
	}

	virtual bool IsBegin() const {
		return TListCollectionIterator::IsBegin();
	}
};

} // namespace JCore