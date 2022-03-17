/*
	�ۼ��� : ������
*/

#pragma once


#include <JCore/Container/VectorIterator.h>
#include <JCore/Container/ArrayCollection.h>

namespace JCore { // namespace JCore

/*=====================================================================================
								  ����
=====================================================================================*/

template <typename T>
class Vector : public ArrayCollection<T>
{
	using TEnumerator			= typename Enumerator<T>;
	using TCollection			= typename Collection<T>;
	using TArrayCollection		= typename ArrayCollection<T>;
	using TVector				= typename Vector<T>;
	using TVectorIterator		= typename VectorIterator<T>;
public:
	Vector(int capacity = TArrayCollection::ms_iDefaultCapcity) 
		: TArrayCollection(capacity, ContainerType::Vector) 
	{
	}

	Vector(const TVector& other) 
		: TArrayCollection(other, ContainerType::Vector) 
	{
	}

	Vector(TVector&& other) 
		: TArrayCollection(Move(other), ContainerType::Vector) 
	{
	}

	Vector(std::initializer_list<T> ilist) 
		: TArrayCollection(ilist, ContainerType::Vector) 
	{
	}

	virtual ~Vector() noexcept {}

public:
	TVector& operator=(const TVector& other) {
		this->CopyFrom(other);
		return *this;
	}

	TVector& operator=(TVector&& other) {
		this->CopyFrom(Move(other));
		return *this;
	}

	TVector& operator=(std::initializer_list<T> ilist) {
		this->CopyFrom(ilist);
		return *this;
	}

	void PushBack(const T& data) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->SetAtUnsafe(this->m_iSize++, data);
	}

	void PushBack(T&& data) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->SetAtUnsafe(this->m_iSize++, data);
	}

	/// <summary>>a
	/// �������� �����͸� �ڿ� �߰��ϰ����� ��
	/// </summary>
	void PushBackAll(const TCollection& collection) {
		int iExpandSize = this->CalculateExpandCapacity(this->m_iSize + collection.Size());

		if (iExpandSize > this->m_iCapacity) {
			this->Expand(iExpandSize);
		}

		TEnumerator it = collection.Begin();
		while (it->HasNext()) {
			this->SetAtUnsafe(this->m_iSize++, it->Next());
		}
	}

	void Insert(int idx, const T& data) {
		// ������ ��ġ�� �����ϴ� ��� �׳� PushBack�� ��������.
		if (idx == this->m_iSize) {
			PushBack(data);
		}

		if (this->IsFull()) {
			this->ExpandAuto();
		}

		int iMoveBlockSize = this->m_iSize - idx;
		
		this->MoveBlock(
			idx, 
			idx + 1, 
			iMoveBlockSize);
		this->SetAt(idx, data);
		this->m_iSize++;
	}

	void Insert(int idx, T&& data) {
		// ������ ��ġ�� �����ϴ� ��� �׳� PushBack�� ��������.
		if (idx == this->m_iSize) {
			PushBack(Move(data));
			return;
		}

		if (this->IsFull()) {
			this->ExpandAuto();
		}

		int iMoveBlockSize = this->m_iSize - idx;

		this->MoveBlock(
			idx,
			idx + 1,
			iMoveBlockSize);
		this->SetAt(idx, Move(data));
		this->m_iSize++;
	}

	/// <summary>
	/// Ư�� ��ġ�� �����͸� ���� �����ϰ��� �� ��
	/// </summary>
	void InsertAll(int idx, const TCollection& collection) {
		if (idx == this->m_iSize) {
			PushBackAll(collection);
			return;
		}

		int iCollectionSize = collection.Size();
		int iExpandSize = this->CalculateExpandCapacity(this->m_iSize + iCollectionSize);

		if (iExpandSize > this->m_iCapacity) {
			this->Expand(iExpandSize);
		}

		int iMoveBlockSize = this->m_iSize - idx;

		this->MoveBlock(
			idx,
			idx + iCollectionSize,
			iMoveBlockSize);

		TEnumerator it = collection.Begin();
		while (it->HasNext()) {
			this->SetAtUnsafe(idx++, it->Next());
		}

		this->m_iSize += iCollectionSize;
	}


	/// <summary>
	/// ���ڸ� �޾Ƽ� ���ο��� �����Ѵ�.
	/// </summary>
	template <typename... Args>
	void EmplaceBack(Args&&... args) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->EmplaceAt(this->m_iSize++, Forward<Args>(args)...);
	}

	/// <summary>
	/// Ư�� ��ġ�� �����̳� ���ο��� �����Ѵ�.
	/// </summary>
	template <typename... Args>
	void EmplaceInsert(int idx, Args&&... args) {
		if (idx == this->m_iSize) {
			EmplaceBack(Forward<Args>(args)...);
			return;
		}

		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->MoveBlock(
			idx,
			idx + 1,
			this->m_iSize - idx);
		this->EmplaceAt(idx, Forward<Args>(args)...);
		this->m_iSize++;
	}

	/// <summary>
	/// ����Ž������ �˻��Ͽ� �����Ͱ� �����ϴ��� Ȯ���Ѵ�.
	/// </summary>
	bool Exist(const T& data) const {
		return Offset(data) >= 0;
	}

	/// <summary>
	/// ����Ž������ �˻��Ͽ� �����Ͱ� �����ϴ��� Ȯ���Ѵ�.
	/// ��, ���ĵ� ���¿��� ���������� �����Ѵ�.
	/// </summary>
	bool ExistBinary(const T& data) const {
		return OffsetBinary(data) >= 0;
	}

	/// <summary>
	/// ����Ž������ �˻��Ͽ� �����Ͱ� ó������ ���۵Ǵ� ��ġ(�ε���)�� ��ȯ�Ѵ�.
	/// </summary>
	int Offset(const T& data) const {
		return Arrays::LinearSearch(this->m_pArray, this->m_iSize, data);
	}

	/// <summary>
	/// �̺�Ž������ �˻��Ͽ� �����Ͱ� ó������ ���۵Ǵ� ��ġ(�ε���)�� ��ȯ�Ѵ�.
	/// ��, ���ĵ� ���¿��� ���������� �����Ѵ�.
	/// </summary>
	int OffsetBinary(const T& data) const {
		int iOffset = Arrays::LowerBound(this->m_pArray, this->m_iSize, data);

		if (iOffset >= this->m_iSize) {
			return -1;
		}

		if (this->m_pArray[iOffset] != data) {
			return -1;
		}

		return iOffset;
	}
	
	/// <summary>
	/// ����Ž������ �����͸� �˻��Ͽ� �����Ѵ�.
	/// </summary>
	bool Remove(const T& data) {
		int iOffset = Offset(data);

		if (iOffset == -1) {
			return false;
		}

		int iMoveBlockSize = this->m_iSize - (iOffset + 1);

		this->DestroyAt(iOffset);
		this->MoveBlock(
			iOffset + 1,
			iOffset,
			iMoveBlockSize);

		this->m_iSize--;
		return true;
	}

	/// <summary>
	/// ����Ž������ �����͸� �˻��Ͽ� �����Ѵ�.
	/// </summary>
	bool RemoveBinary(const T& data) {
		int iOffset = OffsetBinary(data);

		if (iOffset == -1) {
			return false;
		}

		int iMoveBlockSize = this->m_iSize - (iOffset + 1);

		this->DestroyAt(iOffset);
		this->MoveBlock(
			iOffset + 1,
			iOffset,
			iMoveBlockSize);

		this->m_iSize--;
		return true;
	}

	/// <summary>
	/// Ư�� �ε����� �����͸� �����Ѵ�.
	/// </summary>
	void RemoveAt(const int idx) {
		this->ThrowIfIndexIsInvalid(idx);

		int iMoveBlockSize = this->m_iSize - (idx + 1);

		this->DestroyAt(idx);
		this->MoveBlock(
			idx + 1,
			idx,
			iMoveBlockSize);

		this->m_iSize--;
	}

	void Sort() {
		TArrayCollection::Sort(NaturalOrder{});
	}

	template <typename Predicate>
	void Sort(Predicate predicate) {
		TArrayCollection::Sort(predicate);
	}

	void SortRange(const int startIdx, const int endIdx) {
		TArrayCollection::SortRange(startIdx, endIdx, NaturalOrder{});
	}

	template <typename Predicate>
	void SortRange(const int startIdx, const int endIdx, Predicate predicate) {
		TArrayCollection::SortRange(startIdx, endIdx, predicate);
	}

	T& operator[](const int idx) const {
		this->ThrowIfIndexIsInvalid(idx);
		return this->GetAt(idx);
	}

	virtual TEnumerator Begin() const {
		return MakeShared<TVectorIterator>(this->GetOwner(), 0);
	}

	virtual TEnumerator End() const {
		return MakeShared<TVectorIterator>(this->GetOwner(), this->Size());
	}
protected:
	friend class TVectorIterator;
};

} // namespace JCore