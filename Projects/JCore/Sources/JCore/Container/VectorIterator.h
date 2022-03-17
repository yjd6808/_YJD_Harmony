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
	using TArrayCollectionIterator  = typename ArrayCollectionIterator<T>;
	using TVector					= typename Vector<T>;
public:
	VectorIterator(VoidOwner& owner, int pos) : TArrayCollectionIterator(owner, pos) {}
	virtual ~VectorIterator() noexcept {}
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

	virtual bool IsEnd() const {
		return TArrayCollectionIterator::IsEnd();
	}

	virtual bool IsBegin() const {
		return TArrayCollectionIterator::IsBegin();
	}
};

} // namespace JCore