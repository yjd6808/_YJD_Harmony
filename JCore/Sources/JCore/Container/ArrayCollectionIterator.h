/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/Iterator.h>

namespace JCore {

		
// 전방 선언
					class VoidOwner;
template <typename> class ArrayCollection;
template <typename T>
class ArrayCollectionIterator : public Iterator<T>
{
	using TIterator			= typename Iterator<T>;
	using TArrayCollection  = typename ArrayCollection<T>;
public:
	ArrayCollectionIterator(VoidOwner& owner, int pos) : TIterator(owner) {
		m_iPos = pos;
	}
	virtual ~ArrayCollectionIterator() noexcept {}
public:
	virtual bool HasNext() const {
		if (!this->IsValid()) {
			return false;
		}

		return IsValidIndex(m_iPos);
	}

	virtual bool HasPrevious() const {
		if (!this->IsValid()) {
			return false;
		}

		return IsValidIndex(m_iPos - 1);
	}

	virtual T& Next() {
		return CastArrayCollection()->m_pArray[m_iPos++];
	}

	virtual T& Previous() {
		return CastArrayCollection()->m_pArray[--m_iPos];
	}
protected:
	virtual bool IsValidIndex(int idx) const {
		return idx >= 0 && idx < CastArrayCollection()->Size();
	}

	TArrayCollection* CastArrayCollection() const {
		this->ThrowIfIteratorIsNotValid();
		return this->Watcher.Get<TArrayCollection>();
	}
protected:
	int m_iPos;

	friend class TArrayCollection;
};

} // namespace JCore