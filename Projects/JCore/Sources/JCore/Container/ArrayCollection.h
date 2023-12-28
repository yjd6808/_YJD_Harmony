/*
	작성자 : 윤정도
	배열 기반 컨테이너의 추상화 클래스
*/

#pragma once

#include <JCore/Memory.h>
#include <JCore/Comparator.h>

#include <JCore/Container/Arrays.h>
#include <JCore/Container/Collection.h>
#include <JCore/Container/ArrayCollectionIterator.h>


NS_JC_BEGIN

/*=====================================================================================
									다이나믹 배열
					배열 스택, 배열 큐, 배열 리스트의 공통 인터페이스 정의
=====================================================================================*/

template <typename T, typename TAllocator>
class ArrayCollection : public Collection<T, TAllocator>
{
	using TCollection				= Collection<T, TAllocator>;
	using TArrayCollection			= ArrayCollection<T, TAllocator>;
	using TArrayCollectionIterator  = ArrayCollectionIterator<T, TAllocator>;
public:
	// [1]
	ArrayCollection()
		: m_iCapacity(0)
		, m_iSize(0)
		, m_pArray(nullptr)
	{}

	// [2]
	ArrayCollection(int capacity) {
		if (capacity == 0) {
			m_pArray = nullptr;
			m_iSize = 0;
			m_iCapacity = 0;
		} else {
			int iAllocatedSize;
			m_pArray = TAllocator::template AllocateDynamic<T*>(capacity * sizeof(T), iAllocatedSize);
			m_iSize = 0;
			m_iCapacity = capacity;
		}
	}

	
	// [3]
	// Call [2]
	ArrayCollection(const TArrayCollection& other) : TArrayCollection(other.Capacity())	{
		CopyFrom(other);
	}

	// [4]
	// Call [1]
	ArrayCollection(TArrayCollection&& other) noexcept : TArrayCollection() {
		CopyFrom(Move(other));
	}

	// [5]
	// Call [2]
	ArrayCollection(std::initializer_list<T> ilist) noexcept : TArrayCollection(ilist.size() + ms_iDefaultCapacity) {
		CopyFrom(ilist);
	}

	// [6]
	// Call [2]
	ArrayCollection(int capacity, const T& initData) : TArrayCollection(capacity) {
		m_iSize = capacity;

		for (int i = 0; i < capacity; ++i) {
			SetAtUnsafe(i, initData);
		}
	}

	// [7]
	// Call [2]
	ArrayCollection(int capacity, T&& initData) : TArrayCollection(capacity) {
		m_iSize = capacity;

		// Move에는 앞에 3개는 복사 해주고 마지막 1개만 이동해주면 된다.
		// 만약 T가 많이 무거우면 이 작업은 효율이 좋을 것이고
		// T가 엄청 가벼운데 반복문을 이렇게 돌면 효율이 더 안좋을 것 같다.
		for (int i = 0; i < capacity; ++i) {
			SetAtUnsafe(i, i < capacity - 1 ? initData : Move(initData));
		}
	}



	~ArrayCollection() noexcept override = 0;

	int Capacity() const {
		return m_iCapacity;
	}

	/// <summary>
	/// 내부 원소 모두 제거
	/// 
	/// [오버라이딩]
	///  - ArrayQueue
	/// </summary>
	virtual void Clear(bool removeHeap = false) {
		if (m_iSize > 0) {
			DestroyAtRange(0, m_iSize - 1);
			m_iSize = 0;
		}
		
		if (removeHeap) {
			JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(m_pArray, sizeof(T) * m_iCapacity);
		}
	}

	virtual bool Valid() const {
		return m_pArray != nullptr;
	}

	/// <summary>
	/// [오버라이딩]
	///  - ArrayQueue : 한칸이 덜 찬 상태를 꽉찬 상태로 처리해야하기 때문에 오버라이딩 해야함
	/// </summary>
	virtual bool IsFull() const {
		return m_iSize == m_iCapacity;
	}

	T* Source() {
		return m_pArray;
	}

	bool IsEmpty() const override { return m_iSize == 0; }
	int Size() const override { return m_iSize; }
protected:
	/// <summary>
	/// 다른 배열 컨테이너로부터 복사를 받는다.
	/// 
	/// [오버라이딩]
	///  - ArrayQueue
	/// </summary>
	/// <param name="other"></param>
	virtual void CopyFrom(const TArrayCollection& other) {
		DebugAssertMsg(this != &other, "자기 자신에게 대입할 수 없습니다.");

		Clear();

		const int iCapacity = other.Capacity();

		if (iCapacity > m_iCapacity) {
			Expand(iCapacity);
		}

		m_iSize = other.m_iSize;
		m_iCapacity = iCapacity;

		if (other.m_iSize == 0) {
			return;
		}

		CopyElements(this->m_pArray, iCapacity, other.m_pArray, other.m_iSize);
	}

	virtual void CopyFrom(TArrayCollection&& other) {
		// this->ThrowIfAssignSelf(other);

		Clear(true);

		this->m_Owner = Move(other.m_Owner);
		this->m_pArray = other.m_pArray;
		m_iSize = other.m_iSize;
		this->m_iCapacity = other.m_iCapacity;

		other.m_pArray = nullptr;
		other.m_iCapacity = 0;
		other.m_iSize = 0;
	}

	template <bool Reverse = false>
	void CopyElements(T* to, int toCapacity, T* from, int fromCount) {

		// !!!! Emergency !!!!
		// 메모리만 복사해버리면 안된다.
		// 깊은 복사가 발생하도록 대입연산을 수행해줘야한다.
		// Memory::Copy(this->m_pArray, sizeof(T) * iCapacity, other.m_pArray, sizeof(T) * other.m_iSize);
		// 해결책: Vector<String*> 같은건 메모리만 복사해줘도 된다.

		DebugAssert(fromCount <= toCapacity);

		if constexpr (IsPointerType_v<T>) {
			if constexpr (Reverse)
				Memory::CopyReverse(to, sizeof(T) * toCapacity, from, sizeof(T) * fromCount);
			else
				Memory::Copy(to, sizeof(T) * toCapacity, from, sizeof(T) * fromCount);
		} else if constexpr (IsCopyConstructible_v<T>) {	// 복사 생성이 가능해야함
			// 해결책: Vector<String> 같은건 대입 생성을 수행해줘야한다.
			if constexpr (Reverse)
				for (int i = fromCount - 1; i >= 0 && i < toCapacity; --i)
					Memory::PlacementNew(to[i], from[i]);
			else
				for (int i = 0; i < fromCount && i < toCapacity; ++i)
					Memory::PlacementNew(to[i], from[i]);
		} else {
			DebugAssert(false);
		}
	}


	template <bool Reverse = false>
	void MoveElements(T* to, int toCapacity, T* from, int fromCount) {
		// 메모리만 복사해버리면 안된다.
		// 깊은 복사가 발생하도록 대입연산을 수행해줘야한다.
		// Memory::Copy(this->m_pArray, sizeof(T) * iCapacity, other.m_pArray, sizeof(T) * other.m_iSize);
		// 해결책: Vector<String*> 같은건 메모리만 복사해줘도 된다.

		DebugAssert(fromCount <= toCapacity);

		if constexpr (IsPointerType_v<T>) {
			if constexpr (Reverse)
				Memory::CopyReverse(to, sizeof(T) * toCapacity, from, sizeof(T) * fromCount);
			else
				Memory::Copy(to, sizeof(T) * toCapacity, from, sizeof(T) * fromCount);
		} else if constexpr (IsMoveConstructible_v<T>) {	// 복사 생성이 가능해야함
			if constexpr (Reverse)
				for (int i = fromCount - 1; i >= 0 && i < toCapacity; --i)
					Memory::PlacementNew(to[i], Move(from[i]));
			else
				for (int i = 0; i < fromCount && i < toCapacity; ++i)
					Memory::PlacementNew(to[i], Move(from[i]));
		} else {
			DebugAssert(false);
		}
	}


	/// <summary>
	/// 이니셜라이저 리스트로부터의 복사
	/// </summary>
	virtual void CopyFrom(std::initializer_list<T> other) {
		Clear();
		ExpandIfNeeded(int(other.size()));
		m_iSize = int(other.size());

		int i = 0;
		for (const T& otherElem : other) {
			ConstructAt(i++, Move(otherElem));
		}
	}


	template <typename Ty>
	void Fill(int start, int end, Ty&& t) {
		if (end < start) 
			return;

		for (int i = start; i <= end; ++i) {
			ConstructAt(i, Forward<Ty>(t));
		}
	}

	// 용량 수정
	// 만약 현재 용량보다 더 작은 용량을 넣어줄 경우
	template <typename Ty>
	void Resize(int newSize, const Ty& t) {
		if (m_iSize == newSize) {
			return;
		}

		if (newSize > m_iSize) {
			Expand(newSize);
			Fill(m_iSize, newSize - 1, t);
		} else {
			DestroyAtRange(newSize, m_iSize - 1);
		}
		
		m_iSize = newSize;
	}

	// 현재 용량이 전달받은 사이즈를 충분히 커버 가능한지
	virtual bool ExpandIfNeeded(int size) {

		if (size == 0) {
			return false;
		}

		if (size < m_iCapacity) {
			return false;
		}

		const int iCapacity = CalculateExpandCapacity(size);
		Expand(iCapacity);
		return true;
	}

	
	/// <summary>
	/// 현재 용량보다 더 큰 용량으로 확장
	/// 
	/// [오버라이딩]
	/// - ArrayQueue
	/// </summary>
	/// <param name="newCapacity">기존 용량보다 더 큰 값</param>
	virtual void Expand(int newCapacity) {
		if (m_iCapacity >= newCapacity) {
			return;
		}

		DebugAssertMsg(newCapacity > m_iCapacity, "현재 용량보다 더 작은 용량입니다.");
		int iAllocatedSize;
		T* pNewArray = TAllocator::template AllocateDynamic<T*>(newCapacity * sizeof(T), iAllocatedSize);

		if (m_pArray) {
			MoveElements(pNewArray, newCapacity, m_pArray, m_iSize);
			TAllocator::template DeallocateDynamic(m_pArray, sizeof(T) * m_iCapacity);
		}
		
		m_pArray = pNewArray;
		m_iCapacity = newCapacity;
	}

	virtual void Shrink(int newCapacity) {
		if (newCapacity < 0) {
			DebugAssert(false);
			return;
		}

		// 현재 용량이 더 작은 경우 스킵
		if (m_iCapacity <= newCapacity) {
			return;
		}

		// 현재 데이터 수가 용량보다 더 많은 경우 넘친 만큼 삭제
		if (m_iSize > newCapacity) {
			DestroyAtRange(newCapacity, m_iSize - 1);
			m_iSize = newCapacity;
		}

		if (newCapacity == 0) {
			TAllocator::template DeallocateDynamic(m_pArray, sizeof(T) * m_iCapacity);
			m_pArray = nullptr;
			m_iCapacity = 0;
			return;
		}

		int iAllocatedSize;
		T* pNewArray = TAllocator::template AllocateDynamic<T*>(newCapacity * sizeof(T), iAllocatedSize);

		// 기존 데이터 옮김
		if (m_pArray) {
			MoveElements(pNewArray, newCapacity, m_pArray, m_iSize);
			TAllocator::template DeallocateDynamic(m_pArray, sizeof(T) * m_iCapacity);
		}

		m_pArray = pNewArray;
		m_iCapacity = newCapacity;
	}

	// 기존 size보다 ratio 비율만큼 더 크게 용량 맞춤, 디폴트는 size와 완전동일하게
	virtual void ShrinkToFit(float ratio = 1.0f) {
		const int iNewCapacity = int(m_iSize * ratio);
		Shrink(iNewCapacity);
	}
	
	/// <summary>
	/// 사이즈 내부에 존재하는 유효한 인덱스 범위인지
	/// 즉, 데이터가 할당된 위치인지
	/// [오버라이딩]
	///  - ArrayQueue
	/// </summary>
	/// <param name="idx">사이즈 내부의 인덱스</param>
	/// <returns></returns>
	virtual bool IsValidIndex(const int idx) const {
		return idx >= 0 && idx < m_iSize;
	}

	/// <summary>
	/// 할당된 배열 내부에 존재하는 인덱스 값인지
	/// </summary>
	/// <param name="idx">할당된 배열 내부의 인덱스</param>
	virtual bool IsValidIndexCapacity(const int idx) const {
		return idx >= 0 && idx < this->Capacity();
	}

	virtual bool IsValidRange(const int startIdx, const int endIdx) const {
		return startIdx <= endIdx && startIdx >= 0 && endIdx < this->Size();
	}

	virtual bool IsValidRangeCapacity(const int startIdx, const int endIdx) const {
		return startIdx <= endIdx && startIdx >= 0 && endIdx < this->Capacity();
	}

	virtual void ExpandAuto() {
		if (m_iCapacity == 0)
			m_iCapacity = 1;

		Expand(m_iCapacity * ms_iExpandingFactor);
	}

	
	/// <summary>
	/// [오버라이딩]
	///  - ArrayQueue
	/// </summary>
	virtual void DestroyAtRange(const int startIdx, const int endIdx) {
		DebugAssertMsg(this->IsValidRange(startIdx, endIdx),
			"올바르지 않은 인덱스 범위(%d ~ %d) 입니다. (%d, 컨테이너 크기: %d)", startIdx, endIdx, m_iSize);

		// 포인터 타입은 소멸자 호출을 하지 않도록 한다.
		if constexpr (IsPointerType_v<T>)
			return;

		for (int i = startIdx; i <= endIdx; i++) {
			Memory::PlacementDelete(m_pArray[i]);
		}
	}

	template <typename TPredicate>
	void Sort(TPredicate&& predicate) {
		Arrays::Sort(m_pArray, m_iSize, Forward<TPredicate>(predicate));
	}

	void Sort() {
		Arrays::Sort(m_pArray, m_iSize, NaturalOrder{});
	}

	template <typename TPredicate>
	void InsertionSort(TPredicate&& predicate) {
		Arrays::InsertionSort(m_pArray, m_iSize, Forward<TPredicate>(predicate));
	}

	/// <summary>
	/// startIdx 이상 endIdx이하에 위치한 원소들을 정렬한다.
	/// </summary>
	template <typename TPredicate>
	void SortRange(const int startIdx, const int endIdx, TPredicate&& predicate) {
		Arrays::SortRange(m_pArray, startIdx, endIdx, Forward<TPredicate>(predicate));
	}

	void SortRange(const int startIdx, const int endIdx) {
		Arrays::SortRange(m_pArray, startIdx, endIdx, NaturalOrder{});
	}


	T& GetAt(const int idx) const {
		DebugAssertMsg(IsValidIndex(idx), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", idx, m_iSize);
		return m_pArray[idx];
	}


	void SetAt(const int idx, const T& data) {
		DebugAssertMsg(IsValidIndex(idx), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", idx, m_iSize);
		ConstructAt(idx, data);
	}

	void SetAt(const int idx, T&& data) {
		DebugAssertMsg(IsValidIndex(idx), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", idx, m_iSize);
		ConstructAt(idx, Move(data));
	}

	/// <summary>
	/// 만든 이유 : ArrayQueue는 Tail이 다음 삽입될 위치인데 삽입시의 위치가 유효한지 체크할 경우
	///            예외를 던지기 때문에..
	/// </summary>
	void SetAtUnsafe(const int idx, const T& data) noexcept {
		ConstructAt(idx, data);
	}

	void SetAtUnsafe(const int idx, T&& data) noexcept {
		ConstructAt(idx, Move(data));
	}

	template <typename Ty>
	void ConstructAt(const int idx, Ty&& data) {
		if constexpr (IsPointerType_v<T>) {
			m_pArray[idx] = data;
		} else {
			Memory::PlacementNew(m_pArray[idx], Forward<Ty>(data));
		}
	}

	

	template <typename... Args>
	void EmplaceAt(const int idx, Args&&... args) {
		DebugAssertMsg(IsValidIndex(idx), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", idx, m_iSize);

		if constexpr (IsPointerType_v<T>)
			DebugAssertMsg(false, "포인터 타입은 Emplace 기능 사용 금지...");
		else
			Memory::PlacementNew(m_pArray[idx], Forward<Args>(args)...);
	}

	void DestroyAt(const int idx) {
		DebugAssertMsg(IsValidIndex(idx), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", idx, m_iSize);

		// 포인터 타입은 소멸자 호출을 하지 않도록 한다.
		if constexpr (IsPointerType_v<T>)
			return;

		Memory::PlacementDelete(m_pArray[idx]);
	}

	/// <summary>
	/// 블록을 원하는 위치로 이동한다.
	/// 블록은 배열의 특정 인덱스부터 정해진 갯수까지의 구간을 블록이라 한다.
	/// </summary>
	/// <param name="blockIdx"> 블록 시작 위치 </param>
	/// <param name="blockSize"> 블록 크기 </param>
	/// <param name="moveIdx"> 이동할 위치 </param>
	void MoveBlock(const int blockIdx, const int moveIdx, const int blockSize) {
		if (blockSize < 0) {
			throw InvalidArgumentException("복사할 블록 크기가 0보다 작을 수 없습니다.");
		}

		// 제자리 복사는 그냥 반환하자.
		if (moveIdx == blockIdx) {
			return;
		}

		if (blockSize == 0) {
			return;
		}

		// 데이터가 존재하는지
		DebugAssertMsg(IsValidIndex(blockIdx), 
			"(1) 올바르지 않은 데이터 인덱스 입니다. (%d, 컨테이너 크기: %d)", blockIdx, m_iSize);
		DebugAssertMsg(IsValidIndex(blockIdx + blockSize - 1), 
			"(2) 올바르지 않은 데이터 인덱스 입니다. (%d, 컨테이너 크기: %d)", blockIdx + blockSize - 1, m_iSize);

		// 블록이 이동할 위치가 배열 내부에 둘 수 있는지 체크
		DebugAssertMsg(IsValidIndexCapacity(moveIdx),
			"(3) 올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", moveIdx, m_iSize);
		DebugAssertMsg(IsValidIndexCapacity(moveIdx + blockSize - 1),
			"(4) 올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", moveIdx + blockSize - 1, m_iSize);

		

		if (moveIdx > blockIdx) {
			// □□□□□□□□□□□□□■■■■□□□□□□□□
			// └ blockIdx	└ moveIdx
			// └── blocksize ──┘
			// 검게칠해진 겹치는 영역이 있을 수 있으므로 뒤에서부터 복사해줘야함.

			MoveElements<true>(m_pArray + moveIdx, m_iCapacity - moveIdx, m_pArray + blockIdx, blockSize);
			return;
		}

		MoveElements<false>(m_pArray + moveIdx, m_iCapacity - moveIdx, m_pArray + blockIdx, blockSize);
	}


	/// <summary>
	/// 전달받은 사이즈 크기에 맞는 배열 크기를 반환해준다.
	/// </summary>
	int CalculateExpandCapacity(int size) const {
		if (size < m_iCapacity) {
			return m_iCapacity;
		}

		int iExpectedCapacity = m_iCapacity == 0 ? 1 : m_iCapacity;

		while (true) {
			iExpectedCapacity *= ms_iExpandingFactor;
			if (iExpectedCapacity > size) {
				break;
			}
		}
		return iExpectedCapacity;
	}

	CollectionType GetCollectionType() override { return CollectionType::Array; }
protected:
	static constexpr int ms_iExpandingFactor = 4;	// 꽉차면 4배씩 확장
	static constexpr int ms_iDefaultCapacity = 32;	// 초기 배열 크기
protected:
	int m_iCapacity;
	int m_iSize;
	T* m_pArray;

	friend class TArrayCollectionIterator;
};

template <typename T, typename TAllocator>
ArrayCollection<T, TAllocator>::~ArrayCollection() noexcept {}

NS_JC_END