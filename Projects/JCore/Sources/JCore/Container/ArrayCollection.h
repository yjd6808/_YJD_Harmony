/*
	�ۼ��� : ������
	�迭 ��� �����̳��� �߻�ȭ Ŭ����
*/

#pragma once

#include <JCore/Memory.h>
#include <JCore/Comparator.h>

#include <JCore/Container/Arrays.h>
#include <JCore/Container/Collection.h>
#include <JCore/Container/ArrayCollectionIterator.h>


namespace JCore {


/*=====================================================================================
									���̳��� �迭
					�迭 ����, �迭 ť, �迭 ����Ʈ�� ���� �������̽� ����
=====================================================================================*/

template <typename T>
class ArrayCollection : public Collection<T>
{
	using TCollection				= Collection<T>;
	using TArrayCollection			= ArrayCollection<T>;
	using TArrayCollectionIterator  = ArrayCollectionIterator<T>;
public:
	// [1]
	ArrayCollection(ContainerType containerType) 
		: TCollection(CollectionType::Array, containerType)
	{
		m_pArray = nullptr;
		m_iCapacity = 0;
	}

	// [2]
	ArrayCollection(int capacity, ContainerType containerType) 
		: TCollection(CollectionType::Array, containerType)  
	{

		if (capacity < 1) {
			throw InvalidArgumentException("�����̳��� ũ�Ⱑ 0���ϰ� �� �� �����ϴ�.");
		}

		m_pArray = Memory::Allocate<T*>(capacity * sizeof(T));
		m_iCapacity = capacity;
	}

	// [3]
	ArrayCollection(const TArrayCollection& other, ContainerType containerType) 
		: TArrayCollection(other.Capacity(), containerType)	// -> Call [2]
	{
		CopyFrom(other);
	}

	// [4]
	ArrayCollection(TArrayCollection&& other, ContainerType containerType) 
		: TArrayCollection(containerType) // -> Call [1]
	{
		CopyFrom(Move(other));
	}

	// [5]
	ArrayCollection(std::initializer_list<T> ilist, ContainerType containerType)
		: TArrayCollection(ilist.size() + ms_iDefaultCapacity, containerType) // -> Call [2]
	{
		CopyFrom(ilist);
	}

	~ArrayCollection() noexcept override = 0;

	int Capacity() const {
		return m_iCapacity;
	}

	/// <summary>
	/// ���� ���� ��� ����
	/// 
	/// [�������̵�]
	///  - ArrayQueue
	/// </summary>
	virtual void Clear(bool removeHeap = false) {
		if (this->m_iSize == 0) {
			if (removeHeap) {
				DeleteSafe(m_pArray);
			}
			return;
		}


		DestroyAtRange(0, this->m_iSize - 1);
		this->m_iSize = 0;

		if (removeHeap) {
			DeleteSafe(m_pArray);
		}
	}


	/// <summary>
	/// [�������̵�]
	///  - ArrayQueue : ��ĭ�� �� �� ���¸� ���� ���·� ó���ؾ��ϱ� ������ �������̵� �ؾ���
	/// </summary>
	virtual bool IsFull() const {
		return this->m_iSize == m_iCapacity;
	}
protected:
	/// <summary>
	/// �ٸ� �迭 �����̳ʷκ��� ���縦 �޴´�.
	/// 
	/// [�������̵�]
	///  - ArrayQueue
	/// </summary>
	/// <param name="other"></param>
	virtual void CopyFrom(const TArrayCollection& other) {
		this->ThrowIfAssignSelf(other);

		Clear();

		const int iCapacity = other.Capacity();

		if (iCapacity > m_iCapacity) {
			Expand(iCapacity);
		}

		this->m_iSize = other.m_iSize;
		m_iCapacity = iCapacity;
		Memory::Copy(this->m_pArray, sizeof(T) * iCapacity, other.m_pArray, sizeof(T) * other.m_iSize);
	}

	virtual void CopyFrom(TArrayCollection&& other) {
		// this->ThrowIfAssignSelf(other);

		Clear(true);

		this->m_Owner = Move(other.m_Owner);
		this->m_pArray = other.m_pArray;
		this->m_iSize = other.m_iSize;
		this->m_iCapacity = other.m_iCapacity;

		other.m_pArray = nullptr;
		other.m_iSize = 0;
	}

	/// <summary>
	/// �̴ϼȶ����� ����Ʈ�κ����� ����
	/// </summary>
	virtual void CopyFrom(std::initializer_list<T> other) {
		Clear();
		ExpandIfNeeded(other.size());
		this->m_iSize = other.size();
		Memory::CopyUnsafe(m_pArray, other.begin(), sizeof(T) * other.size());
	}

	// �뷮 ����
	virtual void Resize(int capacity) {
		if (capacity >= this->m_iSize) {
			Expand(capacity);
			return;
		} 

		DestroyAtRange(m_iCapacity, this->m_iSize - 1);
		m_iCapacity = capacity;
	}


	virtual bool ExpandIfNeeded(int size) {
		if (size < m_iCapacity) {
			return false;
		}

		const int iCapacity = CalculateExpandCapacity(size);
		Expand(iCapacity);
		return true;
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
	/// ������ ���ο� �����ϴ� ��ȿ�� �ε��� ��������
	/// ��, �����Ͱ� �Ҵ�� ��ġ����
	/// [�������̵�]
	///  - ArrayQueue
	/// </summary>
	/// <param name="idx">������ ������ �ε���</param>
	/// <returns></returns>
	virtual bool IsValidIndex(const int idx) const {
		return idx >= 0 && idx < this->m_iSize;
	}

	/// <summary>
	/// �Ҵ�� �迭 ���ο� �����ϴ� �ε��� ������
	/// </summary>
	/// <param name="idx">�Ҵ�� �迭 ������ �ε���</param>
	virtual bool IsValidIndexCapacity(const int idx) const {
		return idx >= 0 && idx < this->Capacity();
	}

	virtual bool IsValidRange(const int startIdx, const int endIdx) const {
		return startIdx <= endIdx && startIdx >= 0 && endIdx < this->Size();
	}

	virtual bool IsValidRangeCapacity(const int startIdx, const int endIdx) const {
		return startIdx <= endIdx && startIdx >= 0 && endIdx < this->Capacity();
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

	template <typename Predicate>
	void Sort(Predicate predicate) {
		Arrays::Sort(m_pArray, this->m_iSize, predicate);
	}

	void Sort() {
		Arrays::Sort(m_pArray, this->m_iSize, NaturalOrder{});
	}

	/// <summary>
	/// startIdx �̻� endIdx���Ͽ� ��ġ�� ���ҵ��� �����Ѵ�.
	/// </summary>
	template <typename Predicate>
	void SortRange(const int startIdx, const int endIdx, Predicate predicate) {
		Arrays::SortRange(m_pArray, startIdx, endIdx, predicate);
	}

	void SortRange(const int startIdx, const int endIdx) {
		Arrays::SortRange(m_pArray, startIdx, endIdx, NaturalOrder{});
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

	/// <summary>
	/// ����� ���ϴ� ��ġ�� �̵��Ѵ�.
	/// ����� �迭�� Ư�� �ε������� ������ ���������� ������ ����̶� �Ѵ�.
	/// </summary>
	/// <param name="blockIdx"> ��� ���� ��ġ </param>
	/// <param name="blockSize"> ��� ũ�� </param>
	/// <param name="moveIdx"> �̵��� ��ġ </param>
	void MoveBlock(const int blockIdx, const int moveIdx, const int blockSize) {
		if (blockSize < 0) {
			throw InvalidArgumentException("������ ��� ũ�Ⱑ 0���� ���� �� �����ϴ�.");
		}

		// ���ڸ� ����� �׳� ��ȯ����.
		if (moveIdx == blockIdx) {
			return;
		}

		if (blockSize == 0) {
			return;
		}

		// �����Ͱ� �����ϴ���
		ThrowIfIndexIsInvalid(blockIdx);
		ThrowIfIndexIsInvalid(blockIdx + blockSize - 1);

		// ����� �̵��� ��ġ�� �迭 ���ο� �� �� �ִ��� üũ
		ThrowIfIndexIsNotCapacityIndex(moveIdx);
		ThrowIfIndexIsNotCapacityIndex(moveIdx + blockSize - 1);

		if (moveIdx > blockIdx) {
			Memory::CopyUnsafeReverse(
				m_pArray + moveIdx,
				m_pArray + blockIdx,
				blockSize * sizeof(T));
			return;
		}

		Memory::CopyUnsafe(
			m_pArray + moveIdx,
			m_pArray + blockIdx,
			blockSize * sizeof(T));
	}


	/// <summary>
	/// ���޹��� ������ ũ�⿡ �´� �迭 ũ�⸦ ��ȯ���ش�.
	/// </summary>
	int CalculateExpandCapacity(int size) const {
		if (size < m_iCapacity) {
			return m_iCapacity;
		}

		int iExpectedCapacity = m_iCapacity;

		while (true) {
			iExpectedCapacity *= ms_iExpandingFactor;
			if (iExpectedCapacity > size) {
				break;
			}
		}
		return iExpectedCapacity;
	}

	
protected:
	virtual void ThrowIfContainerIsEmpty() const {
		if (this->IsEmpty()) {
			throw InvalidOperationException("�����Ͱ� �������� �ʽ��ϴ�.");
		}
	}

	virtual void ThrowIfIndexIsInvalid(int idx) const {
		if (!IsValidIndex(idx)) {
			throw OutOfRangeException("�ùٸ��� ���� ������ �ε��� �Դϴ�.");
		}
	}

	virtual void ThrowIfIndexIsNotCapacityIndex(int capacityIdx) const {
		if (!IsValidIndexCapacity(capacityIdx)) {
			throw OutOfRangeException("�ùٸ��� ���� �迭 �ε��� �Դϴ�.");
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
	static T* Source(const TArrayCollection& collection) {
		return collection.m_pArray;
	}

	static T& GetAtUnsafe(const TArrayCollection& collection, const int idx) {
		return collection.m_pArray[idx];
	}
protected:
	static constexpr int ms_iExpandingFactor = 4;	// ������ 4�辿 Ȯ��
	static constexpr int ms_iDefaultCapacity = 32;	// �ʱ� �迭 ũ��
protected:
	T* m_pArray;
	int m_iCapacity;

	friend class TArrayCollectionIterator;
};

template <typename T>
ArrayCollection<T>::~ArrayCollection() noexcept {
	Memory::Deallocate(m_pArray);
}

} // namespace JCore