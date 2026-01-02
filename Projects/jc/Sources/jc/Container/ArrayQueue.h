/*
	작성자 : 윤정도
*/

#pragma once

#include <jc/Container/ArrayQueueIterator.h>
#include <jc/Container/ArrayCollection.h>


NS_JC_BEGIN

/*=====================================================================================
							배열 큐
=====================================================================================*/

template <typename T, typename TAllocator = CDefaultAllocator>
class ArrayQueue final : public ArrayCollection<T, TAllocator>
{
	using TEnumerator			= Enumerator<T, TAllocator>;
	using TCollection			= Collection<T, TAllocator>;
	using TArrayCollection		= ArrayCollection<T, TAllocator>;
	using TArrayQueue			= ArrayQueue<T, TAllocator>;
	using TArrayQueueIterator	= ArrayQueueIterator<T, TAllocator>;

public:
	ArrayQueue(int _capacity = TArrayCollection::DEFAULT_CAPACITY)
		: TArrayCollection(_capacity)
	{
	}

	ArrayQueue(const TArrayQueue& _other)
		: TArrayCollection(_other.Capacity())
	{
		operator=(_other);
	}

	ArrayQueue(TArrayQueue&& _other) noexcept
		: TArrayCollection()
	{
		operator=(Move(_other));
	}

	ArrayQueue(std::initializer_list<T> _ilist)
		: TArrayCollection(_ilist)
	{
		operator=(_ilist);
	}

	~ArrayQueue() noexcept override
	{
		Clear(true);
	}

public:
	TArrayQueue& operator=(const TArrayQueue& _other)
	{
		CopyFrom(_other);

		head_ = 0;
		tail_ = _other.Size();
		return *this;
	}

	TArrayQueue& operator=(TArrayQueue&& _other) noexcept
	{
		// this->ThrowIfAssignSelf(other);

		Clear(true);

		this->owner_ = Move(_other.owner_);
		this->pArray_ = _other.pArray_;
		this->size_ = _other.size_;
		this->capacity_ = _other.capacity_;
		head_ = _other.head_;
		tail_ = _other.tail_;

		_other.pArray_ = nullptr;
		_other.size_ = 0;
		return *this;
	}

	TArrayQueue& operator=(std::initializer_list<T> _ilist)
	{
		this->CopyFrom(_ilist);

		head_ = 0;
		tail_ = static_cast<int>(_ilist.size());
		return *this;
	}

	void Enqueue(const T& _data)
	{
		if (this->IsFull())
		{
			this->ExpandAuto();
		}

		++this->size_;
		this->SetAtUnsafe(tail_, _data);
		tail_ = NextTailValue(1);
	}

	void Enqueue(T&& _data)
	{
		if (this->IsFull())
		{
			this->ExpandAuto();
		}

		++this->size_;
		this->SetAtUnsafe(tail_, Move(_data));
		tail_ = NextTailValue(1);
	}

	void EnqueueAll(const TCollection& _collection)
	{
		this->ExpandIfNeeded(this->size_ + _collection.Size());
		this->size_ += _collection.Size();

		// 배열 방식의 컬렉션은 더 효율적인 방식으로 넣어준다.
		if (_collection.GetCollectionType(_collection) == CollectionType::Array)
		{
			EnqueueAllArrayCollection(dynamic_cast<const TArrayCollection&>(_collection));
			return;
		}

		auto iterator = _collection.Begin();

		while (iterator->HasNext())
		{
			this->SetAtUnsafe(tail_, iterator->Next());
			tail_ = NextTailValue(1);
		}
	}

	void Dequeue()
	{
		jc_assert_msg(!this->IsEmpty(), "데이터가 존재하지 않습니다.");
		this->DestroyAt(head_);
		head_ = NextHeadValue(1);
		--this->size_;
	}

	T& Front() const
	{
		jc_assert_msg(!this->IsEmpty(), "데이터가 존재하지 않습니다.");
		return this->pArray_[head_];
	}

	bool TryDequeue(OUT T& _value)
	{
		if (this->size_ > 0)
		{
			_value = Front();
			Dequeue();
			return true;
		}
		return false;
	}

	/*
	T& Back() const {
		this->ThrowIfContainerIsEmpty();
		return this->pArray_[m_iTail];
	}
	*/

	/// <summary>
	/// 큐 내의 원소들 모두 제거
	///
	/// [오버라이딩]
	/// - From CArrayCollection
	/// </summary>
	void Clear(bool _removeHeap = false) override
	{
		if (this->IsEmpty())
		{
			if (_removeHeap)
			{
				JC_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(this->pArray_, sizeof(T) * this->capacity_);
			}
			return;
		}

		if (IsForwardedHead())
		{
			this->DestroyAtRange(head_, this->Capacity() - 1);
			this->DestroyAtRange(0, tail_ - 1);
		}
		else
		{
			this->DestroyAtRange(head_, tail_ - 1);
		}

		this->size_ = 0;

		/*
			원래 아래 대입연산 한번으로 끝내는데

			tail_ = head_;

			큐와 큐간의 복사를 진행하는 CopyFrom() 함수에서 기존 큐는 머리와 꼬리가 0을 가리키고 있는 경우가
			복사를 하기가 제일 쉽기 때문에 0으로 초기화 하도록한다.
		*/

		tail_ = 0;
		head_ = 0;

		if (_removeHeap)
		{
			JC_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(this->pArray_, sizeof(T) * this->capacity_);
		}
	}

	TEnumerator Begin() const override
	{
		return MakeShared<TArrayQueueIterator, TAllocator>(this->GetOwner(), head_);
	}

	// 꼬리위치는 데이터가 삽입될 위치이므로 마지막 원소의 위치는 꼬리에서 1칸 이전의 인덱스이다.
	TEnumerator End() const override
	{
		return MakeShared<TArrayQueueIterator, TAllocator>(this->GetOwner(), tail_);
	}

protected:
	/// <summary>
	///
	/// [오버라이딩]
	///  - From CArrayCollection
	/// </summary>
	void CopyFrom(const TArrayCollection& _arrayCollection) override
	{
		jc_assert_msg(this != &_arrayCollection, "자기 자신에게 대입할 수 없습니다.");

		const TArrayQueue& otherQueue = dynamic_cast<const TArrayQueue&>(_arrayCollection);

		Clear();

		this->ExpandIfNeeded(otherQueue.size_);
		this->size_ = otherQueue.size_;

		if (otherQueue.size_ == 0)
		{
			return;
		}

		if (otherQueue.IsForwardedTail())
		{
			Memory::CopyUnsafe(
				this->pArray_,
				otherQueue.pArray_ + otherQueue.head_,
				sizeof(T) * otherQueue.Size());

			return;
		}

		int headToEndSize = otherQueue.Capacity() - otherQueue.head_;
		const int beginToTailSize = otherQueue.tail_;

		Memory::CopyUnsafe(
			this->pArray_,
			otherQueue.pArray_ + otherQueue.head_,
			sizeof(T) * headToEndSize);

		Memory::CopyUnsafe(
			this->pArray_ + headToEndSize,
			otherQueue.pArray_,
			sizeof(T) * beginToTailSize);
	}

	void CopyFrom(const std::initializer_list<T> _ilist) override
	{
		TArrayCollection::CopyFrom(_ilist);
	}

	// 크기 확장
	void Expand(int _capacity) override
	{
		int allocatedSize = 0;
		T* pNewArray = TAllocator::template AllocateDynamic<T*>(sizeof(T) * _capacity, allocatedSize);

		if (this->pArray_)
		{
			if (IsForwardedTail())
			{
				/*	  아래와 같은 상황에서의 배열 확장방법

					  - : 빈 데이터
					  □ : 데이터

					 ========================================================
					 -----------------□□□□□□□□□□□□□□□□□□□□□□□□□□□------------
					 ========================================================
					 ↑		     ↑                          ↑
					 0	      head                       tail
								 └------ this->Size() ------┘
				*/

				Memory::CopyUnsafe(
					pNewArray,
					this->pArray_ + head_,
					sizeof(T) * this->Size());
			}
			else
			{
				/*	  아래와 같은 상황에서의 배열 확장방법

					  - : 빈 데이터
					  □ : 데이터

					 ========================================================
					 □□□□□□□□□□□□□□□□□□□□□------------------□□□□□□□□□□□□□□□□□
					 ========================================================
					 ↑		         ↑                  ↑
					 0	              tail              head
					 └--beginToTailSize-----┘			└---headToEndSize--┘
				*/

				int headToEndSize = this->Capacity() - head_;
				const int beginToTailSize = tail_;

				Memory::CopyUnsafe(
					pNewArray,
					this->pArray_ + head_,
					sizeof(T) * headToEndSize);

				Memory::CopyUnsafe(
					pNewArray + headToEndSize,
					this->pArray_,
					sizeof(T) * beginToTailSize);
			}

			JC_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(this->pArray_, sizeof(T) * this->capacity_);
		}

		this->pArray_ = pNewArray;
		this->capacity_ = _capacity;

		// 확장 후 꼬리 헤드 위치 변경해줘야함
		head_ = 0;
		tail_ = this->Size();
	}

	/// <summary>
	/// [오버라이딩]
	/// - From CArrayCollection
	///   큐는 용량을 기준으로 유효 인덱스 범위를 판단해야한다.
	/// </summary>
	/// <param name="_startIndex"></param>
	/// <param name="_endIndex"></param>
	bool IsValidRange(int _startIndex, int _endIndex) const override
	{
		return _startIndex <= _endIndex && _startIndex >= 0 && _endIndex < this->Capacity();
	}

	/// <summary>
	/// [오버라이딩]
	/// - From CArrayCollection
	///   큐는 용량을 기준으로 유효 인덱스 범위를 판단해야한다.
	/// </summary>
	bool IsValidIndex(int _index) const override
	{
		if (IsForwardedHead())
		{
			return (_index >= head_ && _index < this->Capacity()) ||
				   (_index >= 0     && _index < tail_);
		}

		return _index >= head_ && _index < tail_;
	}

	/// <summary>
	/// [오버라이딩]
	/// - From CArrayCollection
	/// </summary>
	void DestroyAtRange(const int _startIndex, const int _endIndex) override
	{
		// tail_이 0을 가리키고 있는 경우
		if (_endIndex < 0)
		{
			return;
		}

		TArrayCollection::DestroyAtRange(_startIndex, _endIndex);
	}

	/// <summary>
	/// [오버라이딩]
	///  - From CArrayCollection
	/// </summary>
	bool IsFull() const override
	{
		// 큐는 용량이 없는 경우를 꽉찬 경우로 처리해야한다.
		return this->size_ == this->capacity_ - 1 || this->capacity_ == 0;
	}

protected:
	// 단독으로 호출 금지!
	// EnqueueAll의 부분 함수
	void EnqueueAllArrayCollection(const TArrayCollection& _arrayCollection)
	{
		// 같은 배열 큐인 경우 : 큐는 배열 스택과 벡터와 다른 방식으로 추가해줘야함
		if (_arrayCollection.GetContainerType(_arrayCollection) == ContainerType::ArrayQueue)
		{
			EnqueueAllArrayQueue(dynamic_cast<const TArrayQueue&>(_arrayCollection));
			return;
		}

		for (int index = 0; index < _arrayCollection.Size(); ++index)
		{
			this->SetAtUnsafe(tail_, GetAtUnsafe(_arrayCollection, index));
			tail_ = NextTailValue(1);
		}
	}

	void EnqueueAllArrayQueue(const TArrayQueue& _queue)
	{
		int otherCurrent = _queue.head_;
		const int otherEnd = _queue.tail_;

		while (otherCurrent != otherEnd)
		{
			int otherNext = NextValue(otherCurrent, 1);
			this->SetAtUnsafe(tail_, _queue.pArray_[otherNext]);
			tail_ = NextTailValue(1);
			otherCurrent = otherNext;
		}
	}

	/// <summary>
	/// 꼬리의 gap 만큼 다음 위치에 해당하는 값을 가져온다.
	/// </summary>
	int NextTailValue(int _gap) const
	{
		_gap %= this->Capacity();

		if (tail_ + _gap >= this->Capacity())
		{
			return tail_ + _gap - this->Capacity();
		}

		return tail_ + _gap;
	}

	int NextHeadValue(int _gap) const
	{
		_gap %= this->Capacity();

		if (head_ + _gap >= this->Capacity())
		{
			return head_ + _gap - this->Capacity();
		}

		return head_ + _gap;
	}

	/// <summary>
	/// 꼬리의 gap 만큼 이전 위치에 해당하는 값을 가져온다.
	/// </summary>
	int PrevTailValue(int _gap) const
	{
		_gap %= this->Capacity();

		if (tail_ - _gap < 0)
		{
			return this->Capacity() - (_gap - tail_);
		}

		return tail_ - _gap;
	}

	int PrevHeadValue(int _gap) const
	{
		_gap %= this->Capacity();

		if (head_ - _gap < 0)
		{
			return this->Capacity() - (_gap - head_);
		}

		return head_ - _gap;
	}

	int NextValue(int _value, int _gap)
	{
		_gap %= this->Capacity();

		if (_value + _gap >= this->Capacity())
		{
			return _value + _gap - this->Capacity();
		}

		return _value + _gap;
	}

	// 머리가 꼬리보다 앞서는 상태인 경우
	// 아래에 해당하는 그림의 컨테이너 상태
	bool IsForwardedHead() const
	{
		return tail_ <= head_;
	}

	// 꼬리가 머리보다 앞서는 상태인 경우
	// 위에 해당하는 그림의 컨테이너 상태
	bool IsForwardedTail() const
	{
		return tail_ > head_;
	}

	ContainerType GetContainerType() override
	{
		return ContainerType::ArrayQueue;
	}

protected:
	int head_ = 0; // index inclusive position
	int tail_ = 0; // index exclusive position

	/*
	======================
	□□□□□□□□□□□□□---------
	======================
	↑		     ↑
	head	     tail
   (inclusive)	   (exclusive)

   머리는 시작 인덱스 (초기 : 0) 데이터가 포함되어있기 때문에 inclusive
   꼬리는 마지막 인덱스 + 1로 다음에 삽입될 위치를 나타낸다. 즉 꼬리의 인덱스에는 데이터가 포함되어있지 않기 때문에 exclusive

	*/

	friend class TArrayQueueIterator;
};

NS_JC_END
