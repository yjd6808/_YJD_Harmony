/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Math.h>

#include <JCore/Container/ArrayQueueIterator.h>
#include <JCore/Container/DynamicArray.h>


namespace JCore {

/*=====================================================================================
								배열 스택
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
	
	// 크기 확장
	virtual void Expand(int capacity) {
		T* pNewArray = new T[capacity];

		if (m_iTail > m_iHead) {
			/*	  아래와 같은 상황에서의 배열 확장방법
			
				  - : 빈 데이터
				  □ : 데이터

				 ========================================================
				 -----------------□□□□□□□□□□□□□□□□□□□□□□□□□□□------------
				 ========================================================
				 ↑			     ↑                          ↑
				 0              head                       tail
								 └---- this->GetSize() -----┘

			 */

			Memory::CopyUnsafe(
				pNewArray,
				this->m_pArray + m_iHead,
				sizeof(T) * this->GetSize());
		} else {

			/*	  아래와 같은 상황에서의 배열 확장방법
			
				  - : 빈 데이터
				  □ : 데이터

				 ========================================================
				 □□□□□□□□□□□□□□□□□□□□□------------------□□□□□□□□□□□□□□□□□
				 ========================================================
				 ↑			         ↑                  ↑
				 0                  tail              head
				 └--iBeginToTail-----┘					└---iHeadToEnd--┘

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

		// 확장 후 꼬리 헤드 위치 변경해줘야함
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

	// 머리가 꼬리보다 앞서는 상태인 경우
	// 아래에 해당하는 그림의 컨테이너 상태
	bool IsForwardedHead() const {
		return m_iTail <= m_iHead;
	}

	// 꼬리가 머리보다 앞서는 상태인 경우
	// 위에 해당하는 그림의 컨테이너 상태
	bool IsForwardedTail() const {
		return m_iTail > m_iHead;
	}

protected:
	int m_iHead;
	int m_iTail;

	friend class TArrayQueueIterator;
};

} // namespace JCore