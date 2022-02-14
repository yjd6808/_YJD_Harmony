/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/Math.h>

#include <JCore/Container/ArrayQueueIterator.h>
#include <JCore/Container/ArrayCollection.h>


namespace JCore {

/*=====================================================================================
								�迭 ����
=====================================================================================*/

template <typename T>
class ArrayQueue : public ArrayCollection<T>
{
	using TEnumerator			= typename Enumerator<T>;
	using TArrayCollection		= typename ArrayCollection<T>;
	using TArrayQueue			= typename ArrayQueue<T>;
	using TArrayQueueIterator	= typename ArrayQueueIterator<T>;
public:
	ArrayQueue(int capacity = TArrayCollection::ms_iDefaultCapcity) : TArrayCollection(capacity) {
		m_iHead = 0;
		m_iTail = 0;
	}
	virtual ~ArrayQueue() noexcept {}

	virtual void Enqueue(const T& data) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->m_iSize++;
		this->SetAtUnsafe(m_iTail, data);
		m_iTail = NextTailValue(1);
	}

	virtual void Enqueue(T&& data) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->m_iSize++;
		this->SetAtUnsafe(m_iTail, Move(data));
		m_iTail = NextTailValue(1);
	}

	virtual void Dequeue() {
		this->ThrowIfContainerIsEmpty();
		this->DestroyAt(m_iHead);
		m_iHead = NextHeadValue(1);
		this->m_iSize--;
	}

	T& Front() const {
		this->ThrowIfContainerIsEmpty();
		return this->m_pArray[m_iHead];
	}

	/*
	T& Back() const {
		this->ThrowIfContainerIsEmpty();
		return this->m_pArray[m_iTail];
	}
	*/

	/// <summary>
	/// ť ���� ���ҵ� ��� ����
	/// 
	/// [�������̵�]
	/// - From ArrayCollection
	/// </summary>
	virtual void Clear() {
		if (this->IsEmpty()) {
			return;
		}

		if (IsForwardedHead()) {
			this->DestroyAtRange(m_iHead , this->Capacity() - 2);
			this->DestroyAtRange(0		 , m_iTail - 1);
		} else {
			this->DestroyAtRange(m_iHead, m_iTail - 1);
		}

		this->m_iSize = 0;
		m_iTail = m_iHead;

		// ���� ��ġ�� �Ѵ� �ٲ��� �ʿ䰡 ����.
		// m_iTail = 0;
		// m_iHead = 0;
	}

	virtual TEnumerator Begin() const {
		return MakeShared<TArrayQueueIterator>(this->GetOwner(), m_iHead);
	}

	// ������ġ�� �����Ͱ� ���Ե� ��ġ�̹Ƿ� ������ ������ ��ġ�� �������� 1ĭ ������ �ε����̴�.
	virtual TEnumerator End() const {
		return MakeShared<TArrayQueueIterator>(this->GetOwner(), m_iTail);	
	}

	


protected:
	// ũ�� Ȯ��
	virtual void Expand(int capacity) {
		T* pNewArray = Memory::Allocate<T*>(sizeof(T) * capacity);

		if (m_iTail > m_iHead) {
			/*	  �Ʒ��� ���� ��Ȳ������ �迭 Ȯ����
			
				  - : �� ������
				  �� : ������

				 ========================================================
				 -----------------����������������������������------------
				 ========================================================
				 ��			     ��                          ��
				 0              head                       tail
								 ��---- this->Size() -----��

			 */

			Memory::CopyUnsafe(
				pNewArray,
				this->m_pArray + m_iHead,
				sizeof(T) * this->Size());
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

			int iHeadToEndSize		= this->Capacity() - m_iHead;
			int iBeginToTailSize	= m_iTail; 

			Memory::CopyUnsafe(
				pNewArray,
				this->m_pArray + m_iHead,
				sizeof(T) * iHeadToEndSize);

			Memory::CopyUnsafe(
				pNewArray + iHeadToEndSize,
				this->m_pArray,
				sizeof(T) * iBeginToTailSize);
		}

		Memory::Deallocate(this->m_pArray);

		this->m_pArray = pNewArray;
		this->m_iCapacity = capacity;

		// Ȯ�� �� ���� ��� ��ġ �����������
		m_iHead = 0;
		m_iTail = this->Size();
	}


	/// <summary>
	/// [�������̵�]
	/// - From ArrayCollection
	///   ť�� �뷮�� �������� ��ȿ �ε��� ������ �Ǵ��ؾ��Ѵ�.
	/// </summary>
	/// <param name="startIdx"></param>
	/// <param name="endIdx"></param>
	virtual bool IsValidRange(int startIdx, int endIdx) const {
		return startIdx <= endIdx && startIdx >= 0 && endIdx < this->Capacity();
	}


	/// <summary>
	/// [�������̵�]
	/// - From ArrayCollection
	///   ť�� �뷮�� �������� ��ȿ �ε��� ������ �Ǵ��ؾ��Ѵ�.
	/// </summary>
	/// <param name="startIdx"></param>
	/// <param name="endIdx"></param>
	virtual bool IsValidIndex(int idx) const {
		if (IsForwardedHead()) {
			return (idx >= m_iHead && idx < this->Capacity()) ||
				   (idx >= 0       && idx < m_iTail);
		}

		return idx >= m_iHead && idx < m_iTail;
	}


	/// <summary>
	/// [�������̵�]
	/// - From ArrayCollection
	/// </summary>
	virtual void DestroyAtRange(const int startIdx, const int endIdx) {
		// m_iTail�� 0�� ����Ű�� �ִ� ���
		if (endIdx < 0) {
			return;
		}
		
		TArrayCollection::DestroyAtRange(startIdx, endIdx);
	}

	/// <summary>
	/// [�������̵�]
	///  - From ArrayCollection
	/// </summary>
	virtual bool IsFull() const {
		return this->m_iSize == this->m_iCapacity - 1;
	}

protected:

	/// <summary>
	/// ������ gap ��ŭ ���� ��ġ�� �ش��ϴ� ���� �����´�.
	/// </summary>
	int NextTailValue(int gap) const {
		gap %= this->Capacity();

		if (m_iTail + gap >= this->Capacity()) {
			return m_iTail + gap - this->Capacity();
		}

		return m_iTail + gap;
	}

	int NextHeadValue(int gap) const {
		gap %= this->Capacity();

		if (m_iHead + gap >= this->Capacity()) {
			return m_iHead + gap - this->Capacity();
		}

		return m_iHead + gap;
	}

	/// <summary>
	/// ������ gap ��ŭ ���� ��ġ�� �ش��ϴ� ���� �����´�.
	/// </summary>
	int PrevTailValue(int gap) const {
		gap %= this->Capacity();

		if (m_iTail - gap < 0) {
			return this->Capacity() - (gap - m_iTail);
		}

		return m_iTail - gap;
	}

	int PrevHeadValue(int gap) const {
		gap %= this->Capacity();

		if (m_iHead - gap < 0) {
			return this->Capacity() - (gap - m_iHead);
		}

		return m_iHead - gap;
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
	int m_iHead;		// index inclusive position
	int m_iTail;		// index exclusive position

			/*
			======================
			��������������---------
			======================
			��		     ��
		  head          tail
	   (inclusive)   (exclusive)

	   �Ӹ��� ���� �ε��� (�ʱ� : 0) �����Ͱ� ���ԵǾ��ֱ� ������ inclusive
	   ������ ������ �ε��� + 1�� ������ ���Ե� ��ġ�� ��Ÿ����. �� ������ �ε������� �����Ͱ� ���ԵǾ����� �ʱ� ������ exclusive

	*/

	friend class TArrayQueueIterator;
};

} // namespace JCore