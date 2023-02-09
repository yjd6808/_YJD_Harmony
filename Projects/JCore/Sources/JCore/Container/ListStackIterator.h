/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ListCollectionIterator.h>

NS_JC_BEGIN


// 전방 선언
class  VoidOwner;
template <typename, typename> struct ListNode;
template <typename, typename> class  ListStack;
template <typename T, typename TAllocator>
class ListStackIterator : public ListCollectionIterator<T, TAllocator>
{
	using TListNode					= ListNode<T, TAllocator>;
	using TListCollectionIterator	= ListCollectionIterator<T, TAllocator>;
	using TListStack				= ListStack<T, TAllocator>;
public:
	ListStackIterator(VoidOwner& owner, TListNode* current) : TListCollectionIterator(owner, current) {}
	~ListStackIterator() noexcept override {}
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