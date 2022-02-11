/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/DynamicArrayIterator.h>

namespace JCore {

// 전방 선언
					class VoidOwner;
template <typename> class ArrayStack;
template <typename T>
class ArrayStackIterator : public DynamicArrayIterator<T>
{
	using TDynamicArrayIterator = typename DynamicArrayIterator<T>;
	using TArrayStack			= typename ArrayStack<T>;
public:
	ArrayStackIterator(VoidOwner& owner, int pos) : TDynamicArrayIterator(owner, pos) {}
	virtual ~ArrayStackIterator() noexcept {}
public:
	virtual bool HasValue() const {
		return TDynamicArrayIterator::HasValue();
	}

	virtual T& Next() {
		return TDynamicArrayIterator::Next();
	}

	virtual T& Previous() {
		return TDynamicArrayIterator::Previous();
	}
};

} // namespace JCore