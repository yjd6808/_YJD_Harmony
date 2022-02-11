#pragma once

/*
	�ۼ��� : ������
	�迭 ��� �����̳��� �߻�ȭ Ŭ����
*/

#include <JCore/Container/Collection.h>
#include <JCore/Container/DynamicArrayIterator.h>

namespace JCore {


/*=====================================================================================
									���̳��� �迭
					�迭 ����, �迭 ť, �迭 ����Ʈ�� ���� �������̽� ����
=====================================================================================*/

template <typename T>
class DynamicArray : public Collection<T>
{
	using TDynamicArray			= DynamicArray<T>;
	using TDynamicArrayIterator = DynamicArrayIterator<T>;
public:
	DynamicArray(int capacity) {
		if (capacity < 1) {
			throw InvalidArgumentException("�����̳��� ũ�Ⱑ 0���ϰ� �� �� �����ϴ�.");
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
	// ũ�� Ȯ��
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
			throw InvalidOperationException("�����Ͱ� �������� �ʽ��ϴ�.");
		}
	}

	void ThrowIfIndexIsInvalid(int idx) const {
		if (!IsValidIndex(idx)) {
			throw OutOfRangeException("�ùٸ��� ���� �ε��� �Դϴ�.");
		}
	}

protected:
	static constexpr int ms_iExpandingFactor = 4;	// ������ 4�辿 Ȯ��
	static constexpr int ms_iDefaultCapcity = 32;	// �ʱ� �迭 ũ��
protected:
	T* m_pArray;
	int m_iCapacity;

	friend class TDynamicArrayIterator;
};

} // namespace JCore