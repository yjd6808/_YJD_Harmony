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
	using TListNode					= ListNode<T>;
	using TListCollectionIterator	= ListCollectionIterator<T>;
	using TListQueue				= ListQueue<T>;
public:
	ListQueueIterator(VoidOwner& owner, TListNode* current) : TListCollectionIterator(owner, current) {}
	~ListQueueIterator() noexcept override {}
public:
	bool HasNext() const override {
		return TListCollectionIterator::HasNext();
	}

	bool HasPrevious() const override {
		return TListCollectionIterator::HasPrevious();
	}

	T& Next() override {
		return TListCollectionIterator::Next();
	}

	T& Previous() override {
		return TListCollectionIterator::Previous();
	}

	bool IsEnd() const override {
		return TListCollectionIterator::IsEnd();
	}

	bool IsBegin() const override {
		return TListCollectionIterator::IsBegin();
	}
};

} // namespace JCore