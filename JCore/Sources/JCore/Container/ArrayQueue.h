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
	using TDynamicArray			= typename DynamicArray<T>;
	using TArrayQueue			= typename ArrayQueue<T>;
	using TArrayQueueIterator	= typename ArrayQueueIterator<T>;
public:
	ArrayQueue(int capacity = TDynamicArray::ms_iDefaultCapcity) : TDynamicArray(capacity) {
		m_iHead = 0;
		m_iTail = 0;
	}
	virtual ~ArrayQueue() noexcept {}

	virtual void Enqueue(const T& data) {
		if (this->GetSize() == this->GetCapacity() - 1) {
			this->ExpandAuto();
		}

		this->m_iSize++;
		this->SetAtUnsafe(m_iTail, data);
		m_iTail = NextTailValue(1);
	}

	virtual void Enqueue(T&& data) {
		if (this->GetSize() == this->GetCapacity() - 1) {
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
	/// - From DynamicArray
	/// </summary>
	virtual void Clear() {
		if (this->IsEmpty()) {
			return;
		}

		if (IsForwardedHead()) {
			this->DestroyAtRange(m_iHead , this->GetCapacity() - 1);
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
		return MakeShared<TArrayQueueIterator>(this->GetOwner(), PrevTailValue(1));	
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
		}

		Memory::Deallocate(this->m_pArray);

		this->m_pArray = pNewArray;
		this->m_iCapacity = capacity;

		// Ȯ�� �� ���� ��� ��ġ �����������
		m_iHead = 0;
		m_iTail = this->GetSize();
	}


	/// <summary>
	/// [�������̵�]
	/// - From DynamicArray
	///   ť�� �뷮�� �������� ��ȿ �ε��� ������ �Ǵ��ؾ��Ѵ�.
	/// </summary>
	/// <param name="startIdx"></param>
	/// <param name="endIdx"></param>
	virtual bool IsValidRange(int startIdx, int endIdx) const {
		return startIdx <= endIdx && startIdx >= 0 && endIdx < this->GetCapacity();
	}


	/// <summary>
	/// [�������̵�]
	/// - From DynamicArray
	///   ť�� �뷮�� �������� ��ȿ �ε��� ������ �Ǵ��ؾ��Ѵ�.
	/// </summary>
	/// <param name="startIdx"></param>
	/// <param name="endIdx"></param>
	virtual bool IsValidIndex(int idx) const {
		if (IsForwardedHead()) {
			return (idx >= m_iHead && idx < this->GetCapacity()) ||
				   (idx >= 0       && idx < m_iTail);
		}

		return idx >= m_iHead && idx < m_iTail;
	}


	/// <summary>
	/// [�������̵�]
	/// - From DynamicArray
	/// </summary>
	virtual void DestroyAtRange(const int startIdx, const int endIdx) {
		// m_iTail�� 0�� ����Ű�� �ִ� ���
		if (endIdx < 0) {
			return;
		}
		
		TDynamicArray::DestroyAtRange(startIdx, endIdx);
	}

protected:

	/// <summary>
	/// ������ gap ��ŭ ���� ��ġ�� �ش��ϴ� ���� �����´�.
	/// </summary>
	int NextTailValue(int gap) const {
		gap %= this->GetCapacity();

		if (m_iTail + gap >= this->GetCapacity()) {
			return m_iTail + gap - this->GetCapacity();
		}

		return m_iTail + gap;
	}

	int NextHeadValue(int gap) const {
		gap %= this->GetCapacity();

		if (m_iHead + gap >= this->GetCapacity()) {
			return m_iHead + gap - this->GetCapacity();
		}

		return m_iHead + gap;
	}

	/// <summary>
	/// ������ gap ��ŭ ���� ��ġ�� �ش��ϴ� ���� �����´�.
	/// </summary>
	int PrevTailValue(int gap) const {
		gap %= this->GetCapacity();

		if (m_iTail - gap < 0) {
			return this->GetCapacity() - (gap - m_iTail);
		}

		return m_iTail - gap;
	}

	int PrevHeadValue(int gap) const {
		gap %= this->GetCapacity();

		if (m_iHead - gap < 0) {
			return this->GetCapacity() - (gap - m_iHead);
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