/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/Container/ArrayQueueIterator.h>
#include <JCore/Container/ArrayCollection.h>


namespace JCore {

/*=====================================================================================
								�迭 ����
=====================================================================================*/

template <typename T>
class ArrayQueue : public ArrayCollection<T>
{
	using TEnumerator			= Enumerator<T>;
	using TCollection			= Collection<T>;
	using TArrayCollection		= ArrayCollection<T>;
	using TArrayQueue			= ArrayQueue<T>;
	using TArrayQueueIterator	= ArrayQueueIterator<T>;
public:
	ArrayQueue(int capacity = TArrayCollection::ms_iDefaultCapacity) 
		: TArrayCollection(capacity, ContainerType::ArrayQueue) 
	{
	}

	ArrayQueue(const TArrayQueue& other) 
		: TArrayCollection(other.Capacity(), ContainerType::ArrayQueue) 
	{
		operator=(other);
	}

	ArrayQueue(TArrayQueue&& other) noexcept
		: TArrayCollection(ContainerType::ArrayQueue) 
	{
		operator=(Move(other));
	}

	ArrayQueue(std::initializer_list<T> ilist) 
		: TArrayCollection(ilist, ContainerType::ArrayQueue) 
	{
		operator=(ilist);
	}

	~ArrayQueue() noexcept override {}
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

	virtual void EnqueueAll(const TCollection& collection) {
		this->ExpandIfNeeded(this->m_iSize + collection.Size());
		this->m_iSize += collection.Size();

		// �迭 ����� �÷����� �� ȿ������ ������� �־��ش�.
		if (TCollection::GetCollectionType(collection) == CollectionType::Array) {
			EnqueueAllArrayCollection(dynamic_cast<const TArrayCollection&>(collection));
			return;
		}

		auto it = collection.Begin();

		while (it->HasNext()) {
			this->SetAtUnsafe(m_iTail, it->Next());
			m_iTail = NextTailValue(1);
		}
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
	void Clear(bool removeHeap = false) override {
		if (this->IsEmpty()) {
			if (removeHeap) {
				DeleteSafe(this->m_pArray);
			}
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
			���� �Ʒ� ���Կ��� �ѹ����� �����µ�

			m_iTail = m_iHead;

			ť�� ť���� ���縦 �����ϴ� CopyFrom() �Լ����� ���� ť�� �Ӹ��� ������ 0�� ����Ű�� �ִ� ��찡
			���縦 �ϱⰡ ���� ���� ������ 0���� �ʱ�ȭ �ϵ����Ѵ�.
		*/


		m_iTail = 0;
		m_iHead = 0;

		if (removeHeap) {
			DeleteSafe(this->m_pArray);
		}
	}

	TEnumerator Begin() const override {
		return MakeShared<TArrayQueueIterator>(this->GetOwner(), m_iHead);
	}

	// ������ġ�� �����Ͱ� ���Ե� ��ġ�̹Ƿ� ������ ������ ��ġ�� �������� 1ĭ ������ �ε����̴�.
	TEnumerator End() const override {
		return MakeShared<TArrayQueueIterator>(this->GetOwner(), m_iTail);	
	}

protected:
	/// <summary>
	/// 
	/// [�������̵�]
	///  - From ArrayCollection
	/// </summary>
	void CopyFrom(const TArrayCollection& arrayCollection) override {
		this->ThrowIfAssignSelf(arrayCollection);

		const TArrayQueue& other = dynamic_cast<const TArrayQueue&>(arrayCollection);

		Clear();

		this->ExpandIfNeeded(other.m_iSize);	// Ȯ���� �ʿ��� ��� Ȯ�� ����
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

	// ũ�� Ȯ��
	void Expand(int capacity) override {
		T* pNewArray = Memory::Allocate<T*>(sizeof(T) * capacity);

		if (IsForwardedTail()) {
			/*	  �Ʒ��� ���� ��Ȳ������ �迭 Ȯ����
			
				  - : �� ������
				  �� : ������

				 ========================================================
				 -----------------����������������������������------------
				 ========================================================
				 ��			     ��                          ��
				 0              head                       tail
								 ��------ this->Size() ------��

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
	bool IsValidRange(int startIdx, int endIdx) const override {
		return startIdx <= endIdx && startIdx >= 0 && endIdx < this->Capacity();
	}


	/// <summary>
	/// [�������̵�]
	/// - From ArrayCollection
	///   ť�� �뷮�� �������� ��ȿ �ε��� ������ �Ǵ��ؾ��Ѵ�.
	/// </summary>
	bool IsValidIndex(int idx) const override {
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
	void DestroyAtRange(const int startIdx, const int endIdx) override {
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
	bool IsFull() const override {
		return this->m_iSize == this->m_iCapacity - 1;
	}

protected:
	// �ܵ����� ȣ�� ����!
	// EnqueueAll�� �κ� �Լ�
	void EnqueueAllArrayCollection(const TArrayCollection& arrayCollection) {

		// ���� �迭 ť�� ��� : ť�� �迭 ���ð� ���Ϳ� �ٸ� ������� �߰��������
		if (TCollection::GetContainerType(arrayCollection) == ContainerType::ArrayQueue) {
			EnqueueAllArrayQueue(dynamic_cast<const TArrayQueue&>(arrayCollection));
			return;
		}
		
		for (int i = 0; i < arrayCollection.Size(); i++) {
			this->SetAtUnsafe(m_iTail, TArrayCollection::GetAtUnsafe(arrayCollection, i));
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

	int NextValue(int val, int gap) {
		gap %= this->Capacity();

		if (val + gap >= this->Capacity()) {
			return val + gap - this->Capacity();
		}

		return val + gap;
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
	int m_iHead = 0;		// index inclusive position
	int m_iTail = 0 ;		// index exclusive position

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