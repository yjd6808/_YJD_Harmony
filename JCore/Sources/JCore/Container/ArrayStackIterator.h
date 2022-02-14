/*
	작성자 : 윤정도
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
	using TArrayCollectionIterator  = typename ArrayCollectionIterator<T>;
	using TArrayStack				= typename ArrayStack<T>;
public:
	ArrayStackIterator(VoidOwner& owner, int pos) : TArrayCollectionIterator(owner, pos) {}
	virtual ~ArrayStackIterator() noexcept {}
public:
	virtual bool HasNext() const {
		return TArrayCollectionIterator::HasNext();
	}

	virtual bool HasPrevious() const {
		return TArrayCollectionIterator::HasPrevious();
	}

	virtual T& Next() {
		return TArrayCollectionIterator::Next();
	}

	virtual T& Previous() {
		return TArrayCollectionIterator::Previous();
	}
};

} // namespace JCore