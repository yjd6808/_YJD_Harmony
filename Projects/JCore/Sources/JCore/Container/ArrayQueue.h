/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ArrayQueueIterator.h>
#include <JCore/Container/ArrayCollection.h>


NS_JC_BEGIN

/*=====================================================================================
								배열 큐
=====================================================================================*/

template <typename T, typename TAllocator = DefaultAllocator>
class ArrayQueue final : public ArrayCollection<T, TAllocator>
{
	using TEnumerator			= Enumerator<T, TAllocator>;
	using TCollection			= Collection<T, TAllocator>;
	using TArrayCollection		= ArrayCollection<T, TAllocator>;
	using TArrayQueue			= ArrayQueue<T, TAllocator>;
	using TArrayQueueIterator	= ArrayQueueIterator<T, TAllocator>;
public:
	ArrayQueue(int capacity = TArrayCollection::ms_iDefaultCapacity)  : TArrayCollection(capacity) {
	}

	ArrayQueue(const TArrayQueue& other)  : TArrayCollection(other.Capacity()) {
		operator=(other);
	}

	ArrayQueue(TArrayQueue&& other) noexcept : TArrayCollection() {
		operator=(Move(other));
	}

	ArrayQueue(std::initializer_list<T> ilist) : TArrayCollection(ilist) {
		operator=(ilist);
	}

	~ArrayQueue() noexcept override {
		Clear(true);
	}
public:


	TArrayQueue& operator=(const TArrayQueue& other) {
		CopyFrom(other);

		m_iHead = 0;
		m_iTail = other.Size();
		return *this;
	}

	TArrayQueue& operator=(TArrayQueue&& other) noexcept {
		// this->ThrowIfAssignSelf(other);

		Clear(true);

		this->m_Owner = Move(other.m_Owner);
		this->m_pArray = other.m_pArray;
		this->m_iSize = other.m_iSize;
		this->m_iCapacity = other.m_iCapacity;
		this->m_iHead = other.m_iHead;
		this->m_iTail = other.m_iTail;


		other.m_pArray = nullptr;
		other.m_iSize = 0;
		return *this;
	}

	TArrayQueue& operator=(std::initializer_list<T> ilist) {
		this->CopyFrom(ilist);

		m_iHead = 0;
		m_iTail = ilist.size();
		return *this;
	}

	void Enqueue(const T& data) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		++this->m_iSize;
		this->SetAtUnsafe(m_iTail, data);
		m_iTail = NextTailValue(1);
	}

	void Enqueue(T&& data) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		++this->m_iSize;
		this->SetAtUnsafe(m_iTail, Move(data));
		m_iTail = NextTailValue(1);
	}

	void EnqueueAll(const TCollection& collection) {
		this->ExpandIfNeeded(this->m_iSize + collection.Size());
		this->m_iSize += collection.Size();

		// 배열 방식의 컬렉션은 더 효율적인 방식으로 넣어준다.
		if (collection.GetCollectionType(collection) == CollectionType::Array) {
			EnqueueAllArrayCollection(dynamic_cast<const TArrayCollection&>(collection));
			return;
		}

		auto it = collection.Begin();

		while (it->HasNext()) {
			this->SetAtUnsafe(m_iTail, it->Next());
			m_iTail = NextTailValue(1);
		}
	}

	

	void Dequeue() {
		DebugAssertMsg(!this->IsEmpty(), "데이터가 존재하지 않습니다.");
		this->DestroyAt(m_iHead);
		m_iHead = NextHeadValue(1);
		--this->m_iSize;
	}

	T& Front() const {
		DebugAssertMsg(!this->IsEmpty(), "데이터가 존재하지 않습니다.");
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
	/// - From ArrayCollection
	/// </summary>
	void Clear(bool removeHeap = false) override {
		if (this->IsEmpty()) {
			if (removeHeap) JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(this->m_pArray, sizeof(T) * this->m_iCapacity);
			return;
		}

		if (IsForwardedHead()) {
			this->DestroyAtRange(m_iHead , this->Capacity() - 1);
			this->DestroyAtRange(0		 , m_iTail - 1);
		} else {
			this->DestroyAtRange(m_iHead, m_iTail - 1);
		}

		this->m_iSize = 0;
		
		/*
			원래 아래 대입연산 한번으로 끝내는데

			m_iTail = m_iHead;

			큐와 큐간의 복사를 진행하는 CopyFrom() 함수에서 기존 큐는 머리와 꼬리가 0을 가리키고 있는 경우가
			복사를 하기가 제일 쉽기 때문에 0으로 초기화 하도록한다.
		*/


		m_iTail = 0;
		m_iHead = 0;

		if (removeHeap) JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(this->m_pArray, sizeof(T) * this->m_iCapacity);
	}

	TEnumerator Begin() const override {
		return MakeShared<TArrayQueueIterator, TAllocator>(this->GetOwner(), m_iHead);
	}

	// 꼬리위치는 데이터가 삽입될 위치이므로 마지막 원소의 위치는 꼬리에서 1칸 이전의 인덱스이다.
	TEnumerator End() const override {
		return MakeShared<TArrayQueueIterator, TAllocator>(this->GetOwner(), m_iTail);
	}

protected:
	/// <summary>
	/// 
	/// [오버라이딩]
	///  - From ArrayCollection
	/// </summary>
	void CopyFrom(const TArrayCollection& arrayCollection) override {
		DebugAssertMsg(this != &arrayCollection, "자기 자신에게 대입할 수 없습니다.");

		const TArrayQueue& other = dynamic_cast<const TArrayQueue&>(arrayCollection);

		Clear();

		this->ExpandIfNeeded(other.m_iSize);	// 확장이 필요한 경우 확장 진행
		this->m_iSize = other.m_iSize;

		if (other.IsForwardedTail()) {
			Memory::CopyUnsafe(
				this->m_pArray,
				other.m_pArray + other.m_iHead,
				sizeof(T) * other.Size());

			return;
		}


		int iHeadToEndSize = other.Capacity() - other.m_iHead;
		const int iBeginToTailSize = other.m_iTail;

		Memory::CopyUnsafe(
			this->m_pArray,
			other.m_pArray + other.m_iHead,
			sizeof(T) * iHeadToEndSize);

		Memory::CopyUnsafe(
			this->m_pArray + iHeadToEndSize,
			other.m_pArray,
			sizeof(T) * iBeginToTailSize);
	}

	void CopyFrom(const std::initializer_list<T> ilist) override {
		TArrayCollection::CopyFrom(ilist);
	}

	// 크기 확장
	void Expand(int capacity) override {
		int iAllocated;
		T* pNewArray = TAllocator::template Allocate<T*>(sizeof(T) * capacity, iAllocated);

		if (IsForwardedTail()) {
			/*	  아래와 같은 상황에서의 배열 확장방법
			
				  - : 빈 데이터
				  □ : 데이터

				 ========================================================
				 -----------------□□□□□□□□□□□□□□□□□□□□□□□□□□□------------
				 ========================================================
				 ↑			     ↑                          ↑
				 0              head                       tail
								 └------ this->Size() ------┘

			 */

			Memory::CopyUnsafe(
				pNewArray,
				this->m_pArray + m_iHead,
				sizeof(T) * this->Size());
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

			int iHeadToEndSize		= this->Capacity() - m_iHead;
			const int iBeginToTailSize	= m_iTail; 

			Memory::CopyUnsafe(
				pNewArray,
				this->m_pArray + m_iHead,
				sizeof(T) * iHeadToEndSize);

			Memory::CopyUnsafe(
				pNewArray + iHeadToEndSize,
				this->m_pArray,
				sizeof(T) * iBeginToTailSize);
		}

		JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(this->m_pArray, sizeof(T) * this->m_iCapacity);

		this->m_pArray = pNewArray;
		this->m_iCapacity = capacity;

		// 확장 후 꼬리 헤드 위치 변경해줘야함
		m_iHead = 0;
		m_iTail = this->Size();
	}


	/// <summary>
	/// [오버라이딩]
	/// - From ArrayCollection
	///   큐는 용량을 기준으로 유효 인덱스 범위를 판단해야한다.
	/// </summary>
	/// <param name="startIdx"></param>
	/// <param name="endIdx"></param>
	bool IsValidRange(int startIdx, int endIdx) const override {
		return startIdx <= endIdx && startIdx >= 0 && endIdx < this->Capacity();
	}


	/// <summary>
	/// [오버라이딩]
	/// - From ArrayCollection
	///   큐는 용량을 기준으로 유효 인덱스 범위를 판단해야한다.
	/// </summary>
	bool IsValidIndex(int idx) const override {
		if (IsForwardedHead()) {
			return (idx >= m_iHead && idx < this->Capacity()) ||
				   (idx >= 0       && idx < m_iTail);
		}

		return idx >= m_iHead && idx < m_iTail;
	}


	/// <summary>
	/// [오버라이딩]
	/// - From ArrayCollection
	/// </summary>
	void DestroyAtRange(const int startIdx, const int endIdx) override {
		// m_iTail이 0을 가리키고 있는 경우
		if (endIdx < 0) {
			return;
		}
		
		TArrayCollection::DestroyAtRange(startIdx, endIdx);
	}

	/// <summary>
	/// [오버라이딩]
	///  - From ArrayCollection
	/// </summary>
	bool IsFull() const override {
		return this->m_iSize == this->m_iCapacity - 1;
	}

protected:
	// 단독으로 호출 금지!
	// EnqueueAll의 부분 함수
	void EnqueueAllArrayCollection(const TArrayCollection& arrayCollection) {

		// 같은 배열 큐인 경우 : 큐는 배열 스택과 벡터와 다른 방식으로 추가해줘야함
		if (arrayCollection.GetContainerType(arrayCollection) == ContainerType::ArrayQueue) {
			EnqueueAllArrayQueue(dynamic_cast<const TArrayQueue&>(arrayCollection));
			return;
		}
		
		for (int i = 0; i < arrayCollection.Size(); i++) {
			this->SetAtUnsafe(m_iTail, GetAtUnsafe(arrayCollection, i));
			m_iTail = NextTailValue(1);
		}
	}

	void EnqueueAllArrayQueue(const TArrayQueue& queue) {
		int iOtherCur = queue.m_iHead;
		const int iOtherEnd = queue.m_iTail;

		while (iOtherCur != iOtherEnd) {
			int iOtherNext = NextValue(iOtherCur, 1);
			this->SetAtUnsafe(m_iTail, queue.m_pArray[iOtherNext]);
			m_iTail = NextTailValue(1);
			iOtherCur = iOtherNext;
		}
	}


	/// <summary>
	/// 꼬리의 gap 만큼 다음 위치에 해당하는 값을 가져온다.
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
	/// 꼬리의 gap 만큼 이전 위치에 해당하는 값을 가져온다.
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

	int NextValue(int val, int gap) {
		gap %= this->Capacity();

		if (val + gap >= this->Capacity()) {
			return val + gap - this->Capacity();
		}

		return val + gap;
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

	ContainerType GetContainerType() override { return ContainerType::ArrayQueue; }
protected:
	int m_iHead = 0;		// index inclusive position
	int m_iTail = 0 ;		// index exclusive position

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

NS_JC_END