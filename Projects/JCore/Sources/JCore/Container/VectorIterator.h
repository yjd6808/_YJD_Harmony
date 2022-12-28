/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ArrayCollectionIterator.h>

namespace JCore {

// 전방 선언
class VoidOwner;
template <typename, typename> class Vector;
template <typename T, typename TAllocator>
class VectorIterator : public ArrayCollectionIterator<T, TAllocator>
{
	using TArrayCollectionIterator  = ArrayCollectionIterator<T, TAllocator>;
	using TVector					= Vector<T, TAllocator>;
public:
	VectorIterator(VoidOwner& owner, int pos) : TArrayCollectionIterator(owner, pos) {}
	~VectorIterator() noexcept override = default;
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