#pragma once

/*
	작성자 : 윤정도
	배열 기반 컨테이너의 추상화 클래스
*/

#include <JCore/Container/Collection.h>
#include <JCore/Container/DynamicArrayIterator.h>

namespace JCore {


/*=====================================================================================
									다이나믹 배열
					배열 스택, 배열 큐, 배열 리스트의 공통 인터페이스 정의
=====================================================================================*/

template <typename T>
class DynamicArray : public Collection<T>
{
	using TDynamicArray			= DynamicArray<T>;
	using TDynamicArrayIterator = DynamicArrayIterator<T>;
public:
	DynamicArray(int capacity) {
		if (capacity < 1) {
			throw InvalidArgumentException("컨테이너의 크기가 0이하가 될 수 없습니다.");
		}

		m_pArray = new T[capacity];
		m_iCapacity = capacity;
	}

	virtual ~DynamicArray() noexcept {
		delete[] m_pArray;
	}

	int GetCapacity() const {
		return m_iCapacity;
	}
protected:
	// 크기 확장
	virtual void Expand(int capacity) {
		T* pNewArray = new T[capacity];
		Memory::Copy(pNewArray, sizeof(T) * capacity, m_pArray, sizeof(T) * this->m_iSize);
		delete[] m_pArray;
		m_pArray = pNewArray;
	}

	virtual bool IsValidIndex(const int idx) const {
		return idx >= 0 && idx < this->m_iSize;
	}

	virtual void ExpandAuto() {
		Expand(m_iCapacity * ms_iExpandingFactor);
	}

	T& GetAtUnsafe(const int idx) const {
		return m_pArray[idx];
	}

	T& GetAt(const int idx) const {
		ThrowIfIndexIsInvalid(idx);
		return m_pArray[idx];
	}

	void SetAt(const int idx, const T& data) {
		ThrowIfIndexIsInvalid(idx);
		m_pArray[idx] = data;
	}

	void SetAt(const int idx, T&& data) {
		ThrowIfIndexIsInvalid();
		m_pArray[idx] = Move(data);
	}

	template <typename... Args>
	void EmplaceAt(const int idx, Args&&... args) {
		ThrowIfIndexIsInvalid(idx);
		m_pArray[idx] = T{ Forward<Args>(args)... };
	}

	void DestroyAt(const int idx) {
		ThrowIfIndexIsInvalid(idx);
		m_pArray[idx].~T();
	}

	T& operator[](const int idx) const {
		return GetAt(idx);
	}

	
	
	bool IsFull() const {
		return this->m_iSize == m_iCapacity;
	}

	void ThrowIfContainerIsEmpty() const {
		if (this->IsEmpty()) {
			throw InvalidOperationException("데이터가 존재하지 않습니다.");
		}
	}

	void ThrowIfIndexIsInvalid(int idx) const {
		if (!IsValidIndex(idx)) {
			throw OutOfRangeException("올바르지 않은 인덱스 입니다.");
		}
	}

protected:
	static constexpr int ms_iExpandingFactor = 4;	// 꽉차면 4배씩 확장
	static constexpr int ms_iDefaultCapcity = 32;	// 초기 배열 크기
protected:
	T* m_pArray;
	int m_iCapacity;

	friend class TDynamicArrayIterator;
};

} // namespace JCore