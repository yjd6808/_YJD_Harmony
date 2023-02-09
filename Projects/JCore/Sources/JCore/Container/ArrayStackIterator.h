/*
 *	작성자 : 윤정도
 */

#pragma once

#include <JCore/Container/ArrayCollectionIterator.h>

NS_JC_BEGIN

// 전방 선언
					
template <typename, typename>
class ArrayStack;
class VoidOwner;

template <typename T, typename TAllocator>
class ArrayStackIterator : public ArrayCollectionIterator<T, TAllocator>
{
	using TArrayCollectionIterator  = ArrayCollectionIterator<T, TAllocator>;
	using TArrayStack				= ArrayStack<T, TAllocator>;
public:
	ArrayStackIterator(VoidOwner& owner, int pos) : TArrayCollectionIterator(owner, pos) {}
	~ArrayStackIterator() noexcept override {}
public:
	bool HasNext() const override {
		return TArrayCollectionIterator::HasNext();
	}

	bool HasPrevious() const override {
		return TArrayCollectionIterator::HasPrevious();
	}

	T& Next() override {
		return TArrayCollectionIterator::Next();
	}

	T& Previous() override {
		return TArrayCollectionIterator::Previous();
	}

	T& Current() override {
		return TArrayCollectionIterator::Current();
	}

	bool IsEnd() const override {
		return TArrayCollectionIterator::IsEnd();
	}

	bool IsBegin() const override {
		return TArrayCollectionIterator::IsBegin();
	}
};

NS_JC_END