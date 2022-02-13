
/*
	�ۼ��� : ������
*/

#pragma once


#include <JCore/Container/VectorIterator.h>
#include <JCore/Container/DynamicArray.h>

namespace JCore { // namespace JCore

/*=====================================================================================
								  ����
=====================================================================================*/

template <typename T>
class Vector : public DynamicArray<T>
{
	using TEnumerator			= typename Enumerator<T>;
	using TDynamicArray			= typename DynamicArray<T>;
	using TVector				= typename Vector<T>;
	using TVectorIterator		= typename VectorIterator<T>;
public:
	Vector(int capacity = TDynamicArray::ms_iDefaultCapcity) : TDynamicArray(capacity) {}
	virtual ~Vector() noexcept {}

	void PushBack(const T& data) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->SetAt(this->m_iSize++, data);
	}

	void PushBack(T&& data) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->SetAt(this->m_iSize++, Move(data));
	}

	void Insert(int idx, const T& data) {
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
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->CopyBlock(
			idx,
			idx + 1,
			this->m_iSize - idx - 1);
		this->EmplaceAt(idx, Forward<Args>(args)...);
		this->m_iSize++;
	}

	/// <summary>
	/// ����Ž������ �˻��Ͽ� �����Ͱ� �����ϴ��� Ȯ���Ѵ�.
	/// </summary>
	bool Find(const T& data) const {
		return Offset(data) >= 0;
	}

	/// <summary>
	/// ����Ž������ �˻��Ͽ� �����Ͱ� �����ϴ��� Ȯ���Ѵ�.
	/// ��, ���ĵ� ���¿��� ���������� �����Ѵ�.
	/// </summary>
	bool FindBinary(const T& data) const {
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

	T& operator[](const int idx) const {
		this->ThrowIfIndexIsInvalid(idx);
		return this->GetAt(idx);
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