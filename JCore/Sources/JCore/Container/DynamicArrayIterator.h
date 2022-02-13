
/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/Iterator.h>

namespace JCore {

		
// 전방 선언
					class VoidOwner;
template <typename> class DynamicArray;
template <typename T>
class DynamicArrayIterator : public Iterator<T>
{
	using TIterator		= typename Iterator<T>;
	using TDynamicArray = typename DynamicArray<T>;
public:
	DynamicArrayIterator(VoidOwner& owner, int pos) : TIterator(owner) {
		m_iPos = pos;
	}
	virtual ~DynamicArrayIterator() noexcept {}
public:
	virtual bool HasValue() const {
		if (!this->IsValid()) {
			return false;
		}

		return IsValidIndex(m_iPos);
	}

	virtual T& Next() {
		return CastDynamicArray()->m_pArray[m_iPos++];
	}

	virtual T& Previous() {
		return CastDynamicArray()->m_pArray[m_iPos--];
	}
protected:
	virtual bool IsValidIndex(int idx) const {
		return idx >= 0 && idx < CastDynamicArray()->Size();
	}

	TDynamicArray* CastDynamicArray() const {
		this->ThrowIfIteratorIsNotValid();
		return this->Watcher.Get<TDynamicArray>();
	}
protected:
	int m_iPos;

	friend class TDynamicArray;
};

} // namespace JCore