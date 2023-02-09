/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/Iterator.h>

NS_JC_BEGIN

		
// 전방 선언
class VoidOwner;
template <typename, typename> class ArrayCollection;
template <typename T, typename TAllocator>
class JCORE_NOVTABLE ArrayCollectionIterator : public Iterator<T, TAllocator>
{
	using TIterator			= Iterator<T, TAllocator>;
	using TArrayCollection  = ArrayCollection<T, TAllocator>;
public:
	ArrayCollectionIterator(VoidOwner& owner, int pos)
		: TIterator(owner)
		, m_iPos(pos) {}
	~ArrayCollectionIterator() noexcept override = 0;
public:
	bool HasNext() const override {
		if (!this->IsValid()) {
			return false;
		}

		return IsValidIndex(m_iPos);
	}

	bool HasPrevious() const override {
		if (!this->IsValid()) {
			return false;
		}

		return IsValidIndex(m_iPos - 1);
	}

	T& Next() override {
		return CastArrayCollection()->m_pArray[m_iPos++];
	}

	T& Previous() override {
		return CastArrayCollection()->m_pArray[--m_iPos];
	}

	T& Current() override {
		return CastArrayCollection()->m_pArray[m_iPos];
	}

	bool IsEnd() const override {
		return HasNext() == false;
	}

	bool IsBegin() const override {
		return HasPrevious() == false;
	}
protected:
	virtual bool IsValidIndex(int idx) const {
		return idx >= 0 && idx < CastArrayCollection()->Size();
	}

	TArrayCollection* CastArrayCollection() const {
		this->ThrowIfIteratorIsNotValid();
		return this->Watcher.Get<TArrayCollection*>();
	}
protected:
	int m_iPos;
};

template <typename T, typename TAllocator>
ArrayCollectionIterator<T, TAllocator>::~ArrayCollectionIterator() noexcept {
	// UNUSED
}


NS_JC_END