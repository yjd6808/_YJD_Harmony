/*
	작성자 : 윤정도
	배열 기반 컨테이너의 추상화 클래스
*/

#pragma once

#include <initializer_list>

#include "jc/Assert.h"
#include "jc/Memory.h"
#include "jc/Comparator.h"
#include "jc/Allocator/DefaultAllocator.h"

#include "jc/Container/Arrays.h"
#include "jc/Container/ArrayCollectionIterator.h"


NS_JC_BEGIN

/*=====================================================================================
							다이나믹 배열
				배열 스택, 배열 큐, 배열 리스트의 공통 구현 베이스
=====================================================================================*/

template <typename T, typename TAllocator>
class ArrayCollection
{
	using TArrayCollection			= ArrayCollection<T, TAllocator>;
	using TArrayCollectionIterator	= ArrayCollectionIterator<T, TAllocator, false>;

public:
	// [1]
	ArrayCollection()
		: capacity_(0)
		, size_(0)
		, pArray_(nullptr)
	{}

	// [2]
	ArrayCollection(int _capacity)
	{
		if (_capacity == 0)
		{
			pArray_ = nullptr;
			size_ = 0;
			capacity_ = 0;
		}
		else
		{
			int allocatedSize = 0;
			pArray_ = TAllocator::template AllocateDynamic<T*>(_capacity * sizeof(T), allocatedSize);
			size_ = 0;
			capacity_ = _capacity;
		}
	}

	// [3]
	// Call [2]
	ArrayCollection(const TArrayCollection& _other)
		: TArrayCollection(_other.Capacity())
	{
		CopyFrom(_other);
	}

	// [4]
	// Call [1]
	ArrayCollection(TArrayCollection&& _other) noexcept
		: TArrayCollection()
	{
		CopyFrom(Move(_other));
	}

	// [5]
	// Call [2]
	ArrayCollection(std::initializer_list<T> _ilist) noexcept
		: TArrayCollection(static_cast<int>(_ilist.size()) + DEFAULT_CAPACITY)
	{
		CopyFrom(_ilist);
	}

	// [6]
	// Call [2]
	ArrayCollection(int _capacity, const T& _initData)
		: TArrayCollection(_capacity)
	{
		size_ = _capacity;

		for (int i = 0; i < _capacity; ++i)
		{
			SetAtUnsafe(i, _initData);
		}
	}

	// [7]
	// Call [2]
	ArrayCollection(int _capacity, T&& _initData)
		: TArrayCollection(_capacity)
	{
		size_ = _capacity;

		// Move에는 앞에 3개는 복사 해주고 마지막 1개만 이동해주면 된다.
		// 만약 T가 많이 무거우면 이 작업은 효율이 좋을 것이고
		// T가 엄청 가벼운데 반복문을 이렇게 돌면 효율이 더 안좋을 것 같다.
		for (int i = 0; i < _capacity; ++i)
		{
			SetAtUnsafe(i, i < _capacity - 1 ? _initData : Move(_initData));
		}
	}

	~ArrayCollection() noexcept;

	int Capacity() const
	{
		return capacity_;
	}

	/// <summary>
	/// 내부 원소 모두 제거
	/// </summary>
	void Clear(bool _removeHeap = false)
	{
		if (size_ > 0)
		{
			DestroyAtRange(0, size_ - 1);
			size_ = 0;
		}

		if (_removeHeap)
		{
			JC_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(pArray_, sizeof(T) * capacity_);
		}
	}

	bool Valid() const
	{
		return pArray_ != nullptr;
	}

	bool IsFull() const
	{
		return size_ == capacity_;
	}

	T* Source()
	{
		return pArray_;
	}

	bool IsEmpty() const { return size_ == 0; }
	int Size() const { return size_; }

	TArrayCollectionIterator Begin() const
	{
		return TArrayCollectionIterator(const_cast<TArrayCollection*>(this), pArray_);
	}

	TArrayCollectionIterator End() const
	{
		return TArrayCollectionIterator(const_cast<TArrayCollection*>(this), pArray_ + size_);
	}

protected:
	/// <summary>
	/// 다른 배열 컨테이너로부터 복사를 받는다.
	/// </summary>
	void CopyFrom(const TArrayCollection& _other)
	{
		jc_assert_msg(this != &_other, "자기 자신에게 대입할 수 없습니다.");

		Clear();

		const int capacity = _other.Capacity();

		if (capacity > capacity_)
		{
			Expand(capacity);
		}

		size_ = _other.size_;
		capacity_ = capacity;

		if (_other.size_ == 0)
		{
			return;
		}

		CopyElements(pArray_, capacity, _other.pArray_, _other.size_);
	}

	void CopyFrom(TArrayCollection&& _other)
	{
		// this->ThrowIfAssignSelf(other);

		Clear(true);

		this->pArray_ = _other.pArray_;
		size_ = _other.size_;
		this->capacity_ = _other.capacity_;

		_other.pArray_ = nullptr;
		_other.capacity_ = 0;
		_other.size_ = 0;
	}

	template <bool Reverse = false>
	void CopyElements(T* _pTo, int _toCapacity, T* _pFrom, int _fromCount)
	{
		// !!!! Emergency !!!!
		// 메모리만 복사해버리면 안된다.
		// 깊은 복사가 발생하도록 대입연산을 수행해줘야한다.
		// Memory::Copy(this->m_pArray, sizeof(T) * iCapacity, other.m_pArray, sizeof(T) * other.size_);
		// 해결책: CVector<String*> 같은건 메모리만 복사해줘도 된다.

		jc_assert(_fromCount <= _toCapacity);

		if constexpr (IsPointerType_v<T>)
		{
			if constexpr (Reverse)
			{
				Memory::CopyReverse(_pTo, sizeof(T) * _toCapacity, _pFrom, sizeof(T) * _fromCount);
			}
			else
			{
				Memory::Copy(_pTo, sizeof(T) * _toCapacity, _pFrom, sizeof(T) * _fromCount);
			}
		}
		else if constexpr (IsCopyConstructible_v<T>)
		{
			// 복사 생성이 가능해야함
			// 해결책: CVector<String> 같은건 대입 생성을 수행해줘야한다.
			if constexpr (Reverse)
			{
				for (int i = _fromCount - 1; i >= 0 && i < _toCapacity; --i)
				{
					Memory::PlacementNew(_pTo[i], _pFrom[i]);
				}
			}
			else
			{
				for (int i = 0; i < _fromCount && i < _toCapacity; ++i)
				{
					Memory::PlacementNew(_pTo[i], _pFrom[i]);
				}
			}
		}
		else
		{
			jc_assert(false);
		}
	}

	template <bool Reverse = false>
	void MoveElements(T* _pTo, int _toCapacity, T* _pFrom, int _fromCount)
	{
		// 메모리만 복사해버리면 안된다.
		// 깊은 복사가 발생하도록 대입연산을 수행해줘야한다.
		// Memory::Copy(this->m_pArray, sizeof(T) * iCapacity, other.m_pArray, sizeof(T) * other.size_);
		// 해결책: CVector<String*> 같은건 메모리만 복사해줘도 된다.

		jc_assert(_fromCount <= _toCapacity);

		if constexpr (IsPointerType_v<T>)
		{
			if constexpr (Reverse)
			{
				Memory::CopyReverse(_pTo, sizeof(T) * _toCapacity, _pFrom, sizeof(T) * _fromCount);
			}
			else
			{
				Memory::Copy(_pTo, sizeof(T) * _toCapacity, _pFrom, sizeof(T) * _fromCount);
			}
		}
		else if constexpr (IsMoveConstructible_v<T>)
		{
			// 복사 생성이 가능해야함
			if constexpr (Reverse)
			{
				for (int i = _fromCount - 1; i >= 0 && i < _toCapacity; --i)
				{
					Memory::PlacementNew(_pTo[i], Move(_pFrom[i]));
				}
			}
			else
			{
				for (int i = 0; i < _fromCount && i < _toCapacity; ++i)
				{
					Memory::PlacementNew(_pTo[i], Move(_pFrom[i]));
				}
			}
		}
		else
		{
			jc_assert(false);
		}
	}

	/// <summary>
	/// 이니셜라이저 리스트로부터의 복사
	/// </summary>
	void CopyFrom(std::initializer_list<T> _other)
	{
		Clear();
		ExpandIfNeeded(static_cast<int>(_other.size()));
		size_ = static_cast<int>(_other.size());

		int index = 0;
		for (const T& otherElement : _other)
		{
			ConstructAt(index++, Move(otherElement));
		}
	}

	template <typename Ty>
	void Fill(int _start, int _end, Ty&& _value)
	{
		if (_end < _start)
		{
			return;
		}

		for (int i = _start; i <= _end; ++i)
		{
			ConstructAt(i, Forward<Ty>(_value));
		}
	}

	// 용량 수정
	// 만약 현재 용량보다 더 작은 용량을 넣어줄 경우
	template <typename Ty>
	void Resize(int _newSize, const Ty& _value)
	{
		if (size_ == _newSize)
		{
			return;
		}

		if (_newSize > size_)
		{
			Expand(_newSize);
			Fill(size_, _newSize - 1, _value);
		}
		else
		{
			DestroyAtRange(_newSize, size_ - 1);
		}

		size_ = _newSize;
	}

	// 현재 용량이 전달받은 사이즈를 충분히 커버 가능한지
	bool ExpandIfNeeded(int _size)
	{
		if (_size == 0)
		{
			return false;
		}

		if (_size < capacity_)
		{
			return false;
		}

		const int capacity = CalculateExpandCapacity(_size);
		Expand(capacity);
		return true;
	}

	/// <summary>
	/// 현재 용량보다 더 큰 용량으로 확장
	/// </summary>
	/// <param name="_newCapacity">기존 용량보다 더 큰 값</param>
	void Expand(int _newCapacity)
	{
		if (capacity_ >= _newCapacity)
		{
			return;
		}

		jc_assert_msg(_newCapacity > capacity_, "현재 용량보다 더 작은 용량입니다.");
		int allocatedSize = 0;
		T* pNewArray = TAllocator::template AllocateDynamic<T*>(_newCapacity * sizeof(T), allocatedSize);

		if (pArray_)
		{
			MoveElements(pNewArray, _newCapacity, pArray_, size_);
			TAllocator::template DeallocateDynamic(pArray_, sizeof(T) * capacity_);
		}

		pArray_ = pNewArray;
		capacity_ = _newCapacity;
	}

	void Shrink(int _newCapacity)
	{
		if (_newCapacity < 0)
		{
			jc_assert(false);
			return;
		}

		// 현재 용량이 더 작은 경우 스킵
		if (capacity_ <= _newCapacity)
		{
			return;
		}

		// 현재 데이터 수가 용량보다 더 많은 경우 넘친 만큼 삭제
		if (size_ > _newCapacity)
		{
			DestroyAtRange(_newCapacity, size_ - 1);
			size_ = _newCapacity;
		}

		if (_newCapacity == 0)
		{
			TAllocator::template DeallocateDynamic(pArray_, sizeof(T) * capacity_);
			pArray_ = nullptr;
			capacity_ = 0;
			return;
		}

		int allocatedSize = 0;
		T* pNewArray = TAllocator::template AllocateDynamic<T*>(_newCapacity * sizeof(T), allocatedSize);

		// 기존 데이터 옮김
		if (pArray_)
		{
			MoveElements(pNewArray, _newCapacity, pArray_, size_);
			TAllocator::template DeallocateDynamic(pArray_, sizeof(T) * capacity_);
		}

		pArray_ = pNewArray;
		capacity_ = _newCapacity;
	}

	// 기존 size보다 ratio 비율만큼 더 크게 용량 맞춤, 디폴트는 size와 완전동일하게
	void ShrinkToFit(float _ratio = 1.0f)
	{
		const int newCapacity = static_cast<int>(size_ * _ratio);
		Shrink(newCapacity);
	}

	/// <summary>
	/// 사이즈 내부에 존재하는 유효한 인덱스 범위인지
	/// 즉, 데이터가 할당된 위치인지
	/// </summary>
	bool IsValidIndex(const int _index) const
	{
		return _index >= 0 && _index < size_;
	}

	/// <summary>
	/// 할당된 배열 내부에 존재하는 인덱스 값인지
	/// </summary>
	bool IsValidIndexCapacity(const int _index) const
	{
		return _index >= 0 && _index < Capacity();
	}

	bool IsValidRange(const int _startIndex, const int _endIndex) const
	{
		return _startIndex <= _endIndex && _startIndex >= 0 && _endIndex < Size();
	}

	bool IsValidRangeCapacity(const int _startIndex, const int _endIndex) const
	{
		return _startIndex <= _endIndex && _startIndex >= 0 && _endIndex < Capacity();
	}

	void ExpandAuto()
	{
		if (capacity_ == 0)
		{
			capacity_ = 1;
		}

		Expand(capacity_ * EXPANDING_FACTOR);
	}

	/// <summary>
	/// [startIndex, endIndex] 범위의 원소들을 소멸시킨다.
	/// </summary>
	void DestroyAtRange(const int _startIndex, const int _endIndex)
	{
		jc_assert_msg(IsValidRange(_startIndex, _endIndex),
			"올바르지 않은 인덱스 범위(%d ~ %d) 입니다. (%d, 컨테이너 크기: %d)", _startIndex, _endIndex, size_);

		// 포인터 타입은 소멸자 호출을 하지 않도록 한다.
		if constexpr (IsPointerType_v<T>)
		{
			return;
		}

		for (int i = _startIndex; i <= _endIndex; ++i)
		{
			Memory::PlacementDelete(pArray_[i]);
		}
	}

	template <typename TPredicate>
	void Sort(TPredicate&& _predicate)
	{
		Arrays::Sort(pArray_, size_, Forward<TPredicate>(_predicate));
	}

	void Sort()
	{
		Arrays::Sort(pArray_, size_, NaturalOrder{});
	}

	template <typename TPredicate>
	void InsertionSort(TPredicate&& _predicate)
	{
		Arrays::InsertionSort(pArray_, size_, Forward<TPredicate>(_predicate));
	}

	/// <summary>
	/// startIdx 이상 endIdx이하에 위치한 원소들을 정렬한다.
	/// </summary>
	template <typename TPredicate>
	void SortRange(const int _startIndex, const int _endIndex, TPredicate&& _predicate)
	{
		Arrays::SortRange(pArray_, _startIndex, _endIndex, Forward<TPredicate>(_predicate));
	}

	void SortRange(const int _startIndex, const int _endIndex)
	{
		Arrays::SortRange(pArray_, _startIndex, _endIndex, NaturalOrder{});
	}

	T& GetAt(const int _index) const
	{
		jc_assert_msg(IsValidIndex(_index), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", _index, size_);
		return pArray_[_index];
	}

	void SetAt(const int _index, const T& _data)
	{
		jc_assert_msg(IsValidIndex(_index), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", _index, size_);
		ConstructAt(_index, _data);
	}

	void SetAt(const int _index, T&& _data)
	{
		jc_assert_msg(IsValidIndex(_index), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", _index, size_);
		ConstructAt(_index, Move(_data));
	}

	/// <summary>
	/// 만든 이유 : ArrayQueue는 Tail이 다음 삽입될 위치인데 삽입시의 위치가 유효한지 체크할 경우
	///            예외를 던지기 때문에..
	/// </summary>
	void SetAtUnsafe(const int _index, const T& _data) noexcept
	{
		ConstructAt(_index, _data);
	}

	void SetAtUnsafe(const int _index, T&& _data) noexcept
	{
		ConstructAt(_index, Move(_data));
	}

	template <typename Ty>
	void ConstructAt(const int _index, Ty&& _data)
	{
		if constexpr (IsPointerType_v<T>)
		{
			pArray_[_index] = _data;
		}
		else
		{
			Memory::PlacementNew(pArray_[_index], Forward<Ty>(_data));
		}
	}

	template <typename... Args>
	void EmplaceAt(const int _index, Args&&... _args)
	{
		jc_assert_msg(IsValidIndex(_index), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", _index, size_);

		if constexpr (IsPointerType_v<T>)
		{
			jc_assert_msg(false, "포인터 타입은 Emplace 기능 사용 금지...");
		}
		else
		{
			Memory::PlacementNew(pArray_[_index], Forward<Args>(_args)...);
		}
	}

	void DestroyAt(const int _index)
	{
		jc_assert_msg(IsValidIndex(_index), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", _index, size_);

		// 포인터 타입은 소멸자 호출을 하지 않도록 한다.
		if constexpr (IsPointerType_v<T>)
		{
			return;
		}

		Memory::PlacementDelete(pArray_[_index]);
	}

	/// <summary>
	/// 블록을 원하는 위치로 이동한다.
	/// 블록은 배열의 특정 인덱스부터 정해진 갯수까지의 구간을 블록이라 한다.
	/// </summary>
	/// <param name="_blockIndex"> 블록 시작 위치 </param>
	/// <param name="_blockSize"> 블록 크기 </param>
	/// <param name="_moveIndex"> 이동할 위치 </param>
	void MoveBlock(const int _blockIndex, const int _moveIndex, const int _blockSize)
	{
		if (_blockSize < 0)
		{
			throw InvalidArgumentException("복사할 블록 크기가 0보다 작을 수 없습니다.");
		}

		// 제자리 복사는 그냥 반환하자.
		if (_moveIndex == _blockIndex)
		{
			return;
		}

		if (_blockSize == 0)
		{
			return;
		}

		// 데이터가 존재하는지
		jc_assert_msg(IsValidIndex(_blockIndex),
			"(1) 올바르지 않은 데이터 인덱스 입니다. (%d, 컨테이너 크기: %d)", _blockIndex, size_);
		jc_assert_msg(IsValidIndex(_blockIndex + _blockSize - 1),
			"(2) 올바르지 않은 데이터 인덱스 입니다. (%d, 컨테이너 크기: %d)", _blockIndex + _blockSize - 1, size_);

		// 블록이 이동할 위치가 배열 내부에 둘 수 있는지 체크
		jc_assert_msg(IsValidIndexCapacity(_moveIndex),
			"(3) 올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", _moveIndex, size_);
		jc_assert_msg(IsValidIndexCapacity(_moveIndex + _blockSize - 1),
			"(4) 올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", _moveIndex + _blockSize - 1, size_);

		if (_moveIndex > _blockIndex)
		{
			// □□□□□□□□□□□□□■■■■□□□□□□□□
			// └ blockIdx	└ moveIdx
			// └── blocksize ──┘
			// 검게칠해진 겹치는 영역이 있을 수 있으므로 뒤에서부터 복사해줘야함.

			MoveElements<true>(pArray_ + _moveIndex, capacity_ - _moveIndex, pArray_ + _blockIndex, _blockSize);
			return;
		}

		MoveElements<false>(pArray_ + _moveIndex, capacity_ - _moveIndex, pArray_ + _blockIndex, _blockSize);
	}

	/// <summary>
	/// 전달받은 사이즈 크기에 맞는 배열 크기를 반환해준다.
	/// </summary>
	int CalculateExpandCapacity(int _size) const
	{
		if (_size < capacity_)
		{
			return capacity_;
		}

		int expectedCapacity = capacity_ == 0 ? 1 : capacity_;

		while (true)
		{
			expectedCapacity *= EXPANDING_FACTOR;
			if (expectedCapacity > _size)
			{
				break;
			}
		}

		return expectedCapacity;
	}

protected:
	static constexpr int EXPANDING_FACTOR = 4;   // 꽉차면 4배씩 확장
	static constexpr int DEFAULT_CAPACITY  = 32; // 초기 배열 크기

protected:
	int capacity_;
	int size_;
	T* pArray_;

	template <typename, typename, bool> friend class ArrayCollectionIterator;
};

template <typename T, typename TAllocator>
ArrayCollection<T, TAllocator>::~ArrayCollection() noexcept {}

NS_END
