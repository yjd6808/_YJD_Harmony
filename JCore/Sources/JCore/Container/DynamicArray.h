/*
	�ۼ��� : ������
	�迭 ��� �����̳��� �߻�ȭ Ŭ����
*/

#pragma once

#include <JCore/Memory.h>
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
	using TCollection			= Collection<T>;
	using TDynamicArray			= DynamicArray<T>;
	using TDynamicArrayIterator = DynamicArrayIterator<T>;
public:
	DynamicArray(int capacity) : TCollection()  {
		if (capacity < 1) {
			throw InvalidArgumentException("�����̳��� ũ�Ⱑ 0���ϰ� �� �� �����ϴ�.");
		}

		m_pArray = Memory::Allocate<T*>(sizeof(T) * capacity);
		m_iCapacity = capacity;
	}

	virtual ~DynamicArray() noexcept {
		Memory::Deallocate(m_pArray);
	}

	int Capacity() const {
		return m_iCapacity;
	}

	/// <summary>
	/// ���� ���� ��� ����
	/// 
	/// [�������̵�]
	///  - ArrayQueue
	/// </summary>
	virtual void Clear() {
		DestroyAtRange(0, this->Size() - 1);
		this->m_iSize = 0;
	}

	/// <summary>
	/// [�������̵�]
	///  - ArrayQueue : ��ĭ�� �� �� ���¸� ���� ���·� ó���ؾ��ϱ� ������ �������̵� �ؾ���
	/// </summary>
	virtual bool IsFull() const {
		return this->m_iSize == m_iCapacity;
	}
protected:
	
	// �뷮 ����
	virtual void Resize(int capacity) {
		if (capacity >= this->m_iSize) {
			Expand(capacity);
			return;
		} 

		DestroyAtRange(m_iCapacity, this->m_iSize - 1);
		m_iCapacity = capacity;
	}

	
	/// <summary>
	/// ���� �뷮���� �� ū �뷮���� Ȯ��
	/// 
	/// [�������̵�]
	/// - ArrayQueue
	/// </summary>
	/// <param name="newCapacity">���� �뷮���� �� ū ��</param>
	virtual void Expand(int newCapacity) {
		ThrowIfNewCapacityIsSmallerThanBefore(newCapacity);
		T* pNewArray = Memory::Allocate<T*>(sizeof(T) * newCapacity);
		Memory::Copy(pNewArray, sizeof(T) * newCapacity, m_pArray, sizeof(T) * this->m_iSize);
		Memory::Deallocate(m_pArray);
		m_pArray = pNewArray;
		m_iCapacity = newCapacity;
	}
	
	/// <summary>
	/// ��ȿ�� �ε��� ��������
	/// 
	/// [�������̵�]
	///  - ArrayQueue
	/// </summary>
	/// <param name="idx">������ ������ �ε���</param>
	/// <returns></returns>
	virtual bool IsValidIndex(const int idx) const {
		return idx >= 0 && idx < this->m_iSize;
	}

	virtual bool IsValidRange(const int startIdx, const int endIdx) const {
		return startIdx <= endIdx && startIdx >= 0 && endIdx < this->Size();
	}

	virtual void ExpandAuto() {
		Expand(m_iCapacity * ms_iExpandingFactor);
	}

	
	/// <summary>
	/// [�������̵�]
	///  - ArrayQueue
	/// </summary>
	virtual void DestroyAtRange(const int startIdx, const int endIdx) {
		ThrowIfRangeIsInvalid(startIdx, endIdx);

		for (int i = startIdx; i <= endIdx; i++) {
			Memory::PlacementDeallocate(m_pArray[i]);
		}
	}
protected:
	T& GetAt(const int idx) const {
		ThrowIfIndexIsInvalid(idx);
		return m_pArray[idx];
	}

	void SetAt(const int idx, const T& data) {
		ThrowIfIndexIsInvalid(idx);
		Memory::PlacementAllocate(m_pArray[idx]);
		m_pArray[idx] = data;
	}

	/// <summary>
	/// ���� ���� : ArrayQueue�� Tail�� ���� ���Ե� ��ġ�ε� ���Խ��� ��ġ�� ��ȿ���� üũ�� ���
	///            ���ܸ� ������ ������..
	/// </summary>
	void SetAtUnsafe(const int idx, const T& data) noexcept {
		Memory::PlacementAllocate(m_pArray[idx]);
		m_pArray[idx] = data;
	}

	void SetAtUnsafe(const int idx, T&& data) noexcept {
		Memory::PlacementAllocate(m_pArray[idx]);
		m_pArray[idx] = Move(data);
	}

	

	void SetAt(const int idx, T&& data) {
		Memory::PlacementAllocate(m_pArray[idx]);
		m_pArray[idx] = Move(data);
	}

	template <typename... Args>
	void EmplaceAt(const int idx, Args&&... args) {
		ThrowIfIndexIsInvalid(idx);
		Memory::PlacementAllocate(m_pArray[idx], Forward<Args>(args)...);
	}

	void DestroyAt(const int idx) {
		ThrowIfIndexIsInvalid(idx);
		Memory::PlacementDeallocate(m_pArray[idx]);
	}

	

	T& operator[](const int idx) const {
		return GetAt(idx);
	}

	

protected:
	virtual void ThrowIfContainerIsEmpty() const {
		if (this->IsEmpty()) {
			throw InvalidOperationException("�����Ͱ� �������� �ʽ��ϴ�.");
		}
	}

	virtual void ThrowIfIndexIsInvalid(int idx) const {
		if (!IsValidIndex(idx)) {
			throw OutOfRangeException("�ùٸ��� ���� �ε��� �Դϴ�.");
		}
	}

	virtual void ThrowIfRangeIsInvalid(int startIdx, int endIdx) const {
		if (!IsValidRange(startIdx, endIdx)) {
			throw OutOfRangeException("�ùٸ��� ���� �ε��� �����Դϴ�.");
		}
	}

	virtual void ThrowIfNewCapacityIsSmallerThanBefore(int newCapacity) const {
		if (newCapacity < m_iCapacity) {
			throw InvalidArgumentException("���� �뷮���� �� ���� �뷮�Դϴ�.");
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