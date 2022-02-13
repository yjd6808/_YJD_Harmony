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
	/// 큐 내의 원소들 모두 제거
	/// 
	/// [오버라이딩]
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

		// 굳이 위치를 둘다 바꿔줄 필요가 없다.
		// m_iTail = 0;
		// m_iHead = 0;
	}

	virtual TEnumerator Begin() const {
		return MakeShared<TArrayQueueIterator>(this->GetOwner(), m_iHead);
	}

	// 꼬리위치는 데이터가 삽입될 위치이므로 마지막 원소의 위치는 꼬리에서 1칸 이전의 인덱스이다.
	virtual TEnumerator End() const {
		return MakeShared<TArrayQueueIterator>(this->GetOwner(), PrevTailValue(1));	
	}

	


protected:
	// 크기 확장
	virtual void Expand(int capacity) {
		T* pNewArray = Memory::Allocate<T*>(sizeof(T) * capacity);

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
		}

		Memory::Deallocate(this->m_pArray);

		this->m_pArray = pNewArray;
		this->m_iCapacity = capacity;

		// 확장 후 꼬리 헤드 위치 변경해줘야함
		m_iHead = 0;
		m_iTail = this->GetSize();
	}


	/// <summary>
	/// [오버라이딩]
	/// - From DynamicArray
	///   큐는 용량을 기준으로 유효 인덱스 범위를 판단해야한다.
	/// </summary>
	/// <param name="startIdx"></param>
	/// <param name="endIdx"></param>
	virtual bool IsValidRange(int startIdx, int endIdx) const {
		return startIdx <= endIdx && startIdx >= 0 && endIdx < this->GetCapacity();
	}


	/// <summary>
	/// [오버라이딩]
	/// - From DynamicArray
	///   큐는 용량을 기준으로 유효 인덱스 범위를 판단해야한다.
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
	/// [오버라이딩]
	/// - From DynamicArray
	/// </summary>
	virtual void DestroyAtRange(const int startIdx, const int endIdx) {
		// m_iTail이 0을 가리키고 있는 경우
		if (endIdx < 0) {
			return;
		}
		
		TDynamicArray::DestroyAtRange(startIdx, endIdx);
	}

protected:

	/// <summary>
	/// 꼬리의 gap 만큼 다음 위치에 해당하는 값을 가져온다.
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
	/// 꼬리의 gap 만큼 이전 위치에 해당하는 값을 가져온다.
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
	int m_iHead;		// index inclusive position
	int m_iTail;		// index exclusive position

			/*
			======================
			□□□□□□□□□□□□□---------
			======================
			↑		     ↑
		  head          tail
	   (inclusive)   (exclusive)

	   머리는 시작 인덱스 (초기 : 0) 데이터가 포함되어있기 때문에 inclusive
	   꼬리는 마지막 인덱스 + 1로 다음에 삽입될 위치를 나타낸다. 즉 꼬리의 인덱스에는 데이터가 포함되어있지 않기 때문에 exclusive

	*/

	friend class TArrayQueueIterator;
};

} // namespace JCore