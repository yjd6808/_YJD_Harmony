/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ListCollectionIterator.h>

NS_JC_BEGIN


// 전방 선언
class  VoidOwner;
template <typename, typename> struct ListNode;
template <typename, typename> class  ListQueue;
template <typename T, typename TAllocator>
class ListQueueIterator : public ListCollectionIterator<T, TAllocator>
{
	using TListNode					= ListNode<T, TAllocator>;
	using TListCollectionIterator	= ListCollectionIterator<T, TAllocator>;
	using TListQueue				= ListQueue<T, TAllocator>;
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

	T& Current() override {
		return TListCollectionIterator::Current();
	}

	bool IsEnd() const override {
		return TListCollectionIterator::IsEnd();
	}

	bool IsBegin() const override {
		return TListCollectionIterator::IsBegin();
	}
};

NS_JC_END