/*
 *	작성자 : 윤정도
 */

#pragma once

#include <JCore/Container/ArrayCollectionIterator.h>

namespace JCore {

// 전방 선언
					class VoidOwner;
template <typename> class ArrayStack;
template <typename T>
class ArrayStackIterator : public ArrayCollectionIterator<T>
{
	using TArrayCollectionIterator  = ArrayCollectionIterator<T>;
	using TArrayStack				= ArrayStack<T>;
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

	bool IsEnd() const override {
		return TArrayCollectionIterator::IsEnd();
	}

	bool IsBegin() const override {
		return TArrayCollectionIterator::IsBegin();
	}
};

} // namespace JCore