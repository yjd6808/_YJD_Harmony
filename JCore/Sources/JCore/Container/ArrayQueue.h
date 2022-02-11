/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/Math.h>

#include <JCore/Container/ArrayQueueIterator.h>
#include <JCore/Container/DynamicArray.h>


namespace JCore {

/*=====================================================================================
								�迭 ����
=====================================================================================*/

template <typename T>
class ArrayQueue : public DynamicArray<T>
{
	using TEnumerator			= typename Enumerator<T>;
	using TArrayQueue			= typename ArrayQueue<T>;
	using TDynamicArray			= typename DynamicArray<T>;
	using TArrayQueueIterator	= typename ArrayQueueIterator<T>;
public:
	ArrayQueue(int capacity = TDynamicArray::ms_iDefaultCapcity) : TDynamicArray(capacity) {
		m_iHead = 0;
		m_iTail = 0;
	}
	virtual ~ArrayQueue() noexcept {}

	virtual void Enqueue(const T& data) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->m_iSize++;
		this->SetAt(m_iTail++, data);
		if (m_iTail == this->GetCapacity()) {
			m_iTail = 0;
		}
	}

	virtual void Dequeue() {
		this->ThrowIfContainerIsEmpty();
		this->DestroyAt(m_iHead);

		m_iHead++;

		if (m_iHead == this->GetCapacity()) {
			m_iHead = 0;
		}

		
		this->m_iSize--;
	}

	T& Front() const {
		this->ThrowIfContainerIsEmpty();
		return this->m_pArray[m_iHead];
	}

	T& Back() const {
		this->ThrowIfContainerIsEmpty();
		return this->m_pArray[m_iTail];
	}

	virtual TEnumerator Begin() const {
		return MakeShared<TArrayQueueIterator>(this->GetOwner(), m_iHead);
	}

	virtual TEnumerator End() const {
		return MakeShared<TArrayQueueIterator>(this->GetOwner(), m_iTail);
	}

protected:
	
	// ũ�� Ȯ��
	virtual void Expand(int capacity) {
		T* pNewArray = new T[capacity];

		if (m_iTail > m_iHead) {
			/*	  �Ʒ��� ���� ��Ȳ������ �迭 Ȯ����
			
				  - : �� ������
				  �� : ������

				 ========================================================
				 -----------------����������������������������------------
				 ========================================================
				 ��			     ��                          ��
				 0              head                       tail
								 ��---- this->GetSize() -----��

			 */

			Memory::CopyUnsafe(
				pNewArray,
				this->m_pArray + m_iHead,
				sizeof(T) * this->GetSize());
		} else {

			/*	  �Ʒ��� ���� ��Ȳ������ �迭 Ȯ����
			
				  - : �� ������
				  �� : ������

				 ========================================================
				 ����������������������------------------������������������
				 ========================================================
				 ��			         ��                  ��
				 0                  tail              head
				 ��--iBeginToTail-----��					��---iHeadToEnd--��

			 */

			int iHeadToEndSize		= this->GetCapacity() - m_iHead;
			int iBeginToTailSize	= m_iTail; 

			Memory::CopyUnsafe(
				pNewArray,
				this->m_pArray + m_iHead,
				sizeof(T) * iHeadToEndSize);

			Memory::CopyUnsafe(
				pNewArray + iHeadToEndSize,
				this->m_pArray,
				sizeof(T) * iBeginToTailSize);

			auto z1 = pNewArray[10];
			auto z2 = pNewArray[11];
			auto z3 = pNewArray[12];
			auto z4 = pNewArray[13];
			auto z5 = pNewArray[14];
			auto z6 = pNewArray[15];

			auto z7 = pNewArray[30];
			auto z8 = pNewArray[31];
		}


		delete[] this->m_pArray;

		this->m_pArray = pNewArray;
		this->m_iCapacity = capacity;

		// Ȯ�� �� ���� ��� ��ġ �����������
		m_iHead = 0;
		m_iTail = this->GetSize();
	}

	virtual bool IsValidIndex(int idx) const {
		if (IsForwardedHead()) {
			return (idx >= m_iHead && idx < this->GetCapacity()) ||
				   (idx >= 0       && idx < m_iTail);
		}

		return idx >= m_iHead && idx < m_iTail;
	}

	// �Ӹ��� �������� �ռ��� ������ ���
	// �Ʒ��� �ش��ϴ� �׸��� �����̳� ����
	bool IsForwardedHead() const {
		return m_iTail <= m_iHead;
	}

	// ������ �Ӹ����� �ռ��� ������ ���
	// ���� �ش��ϴ� �׸��� �����̳� ����
	bool IsForwardedTail() const {
		return m_iTail > m_iHead;
	}

protected:
	int m_iHead;
	int m_iTail;

	friend class TArrayQueueIterator;
};

} // namespace JCore