
/*
	¿€º∫¿⁄ : ¿±¡§µµ
*/

#pragma once


#include <JCore/Container/VectorIterator.h>
#include <JCore/Container/AbstractDynamicArray.h>

namespace JCore { // namespace JCore

/*=====================================================================================
								  ∫§≈Õ
=====================================================================================*/

template <typename T>
class Vector : public AbstractDynamicArray<T>
{
	using TEnumerator			= typename Enumerator<T>;
	using TAbstractDynamicArray			= typename AbstractDynamicArray<T>;
	using TVector				= typename Vector<T>;
	using TVectorIterator		= typename VectorIterator<T>;
public:
	Vector(int capacity = TAbstractDynamicArray::ms_iDefaultCapcity) : TAbstractDynamicArray(capacity) {}
	virtual ~Vector() noexcept {}

	void PushBack(const T& data) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->SetAt(this->m_iSize++, data);
	}

	void PushBack(const T&& data) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->SetAt(this->m_iSize++, Move(data));
	}

	template <typename... Args>
	void EmplaceBack(Args&&... args) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->EmplaceAt(this->m_iSize++, Forward<Args>(args)...);
	}

	virtual TEnumerator Begin() const {
		return MakeShared<TVectorIterator>(this->GetOwner(), 0);
	}

	virtual TEnumerator End() const {
		return MakeShared<TVectorIterator>(this->GetOwner(), this->Size() - 1);
	}

protected:
	friend class TVectorIterator;
};

} // namespace JCore