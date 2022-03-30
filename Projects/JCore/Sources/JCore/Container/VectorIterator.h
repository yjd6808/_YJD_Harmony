/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ArrayCollectionIterator.h>

namespace JCore {

// 전방 선언
					class VoidOwner;
template <typename> class Vector;
template <typename T>
class VectorIterator : public ArrayCollectionIterator<T>
{
	using TArrayCollectionIterator  = ArrayCollectionIterator<T>;
	using TVector					= Vector<T>;
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