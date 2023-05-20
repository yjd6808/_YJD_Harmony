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
	ArrayCollection() : TCollection() {
		m_pArray = nullptr;
		m_iCapacity = 0;
	}

	// [2]
	ArrayCollection(int capacity) : TCollection() {
		DebugAssertMsg(capacity >= 1, "컨테이너의 크기가 0이하가 될 수 없습니다. (%d)", capacity);
		int iAllocatedSize;
		m_pArray = TAllocator::template Allocate<T*>(capacity * sizeof(T), iAllocatedSize);
		m_iCapacity = capacity;
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
		this->m_iSize = capacity;

		for (int i = 0; i < capacity; ++i) {
			SetAtUnsafe(i, initData);
		}
	}

	// [7]
	// Call [2]
	ArrayCollection(int capacity, T&& initData) : TArrayCollection(capacity) {
		this->m_iSize = capacity;

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
		if (this->m_iSize > 0) DestroyAtRange(0, this->m_iSize - 1);
		this->m_iSize = 0;
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
		return this->m_iSize == m_iCapacity;
	}
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

		this->m_iSize = other.m_iSize;
		m_iCapacity = iCapacity;


		// !!!! Emergency !!!!
		// 메모리만 복사해버리면 안된다.
		// 깊은 복사가 발생하도록 대입연산을 수행해줘야한다.
		// Memory::Copy(this->m_pArray, sizeof(T) * iCapacity, other.m_pArray, sizeof(T) * other.m_iSize);


		// 해결책: Vector<String*> 같은건 메모리만 복사해줘도 된다.
		if constexpr (IsPointerType_v<T>) {
			Memory::Copy(this->m_pArray, sizeof(T) * iCapacity, other.m_pArray, sizeof(T) * other.m_iSize);
		} else if constexpr (IsCopyConstructible_v<T>) {	// 복사 생성이 가능해야함
			// 해결책: Vector<String> 같은건 대입 생성을 수행해줘야한다.
			for (int i = 0; i < other.m_iSize; ++i) {
				Memory::PlacementNew(m_pArray[i], other.m_pArray[i]);
			}
		}
	}

	virtual void CopyFrom(TArrayCollection&& other) {
		// this->ThrowIfAssignSelf(other);

		Clear(true);

		this->m_Owner = Move(other.m_Owner);
		this->m_pArray = other.m_pArray;
		this->m_iSize = other.m_iSize;
		this->m_iCapacity = other.m_iCapacity;

		other.m_pArray = nullptr;
		other.m_iCapacity = 0;
		other.m_iSize = 0;
	}

	/// <summary>
	/// 이니셜라이저 리스트로부터의 복사
	/// </summary>
	virtual void CopyFrom(std::initializer_list<T> other) {
		Clear();
		ExpandIfNeeded(other.size());
		this->m_iSize = other.size();
		Memory::CopyUnsafe(m_pArray, other.begin(), sizeof(T) * other.size());
	}

	// 용량 수정
	// 만약 현재 용량보다 더 작은 용량을 넣어줄 경우
	virtual void Resize(int capacity) {
		if (capacity >= this->m_iSize) {
			Expand(capacity);
			return;
		} 

		DestroyAtRange(capacity, this->m_iSize - 1);
		m_iCapacity = capacity;
	}

	virtual bool ExpandIfNeeded(int size) {
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
		DebugAssertMsg(newCapacity > m_iCapacity, "현재 용량보다 더 작은 용량입니다.");
		int iAllocatedSize;
		T* pNewArray = TAllocator::template Allocate<T*>(newCapacity * sizeof(T), iAllocatedSize);
		Memory::Copy(pNewArray, sizeof(T) * newCapacity, m_pArray, sizeof(T) * this->m_iSize);
		TAllocator::template Deallocate(m_pArray, sizeof(T) * m_iCapacity);
		m_pArray = pNewArray;
		m_iCapacity = newCapacity;
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
		return idx >= 0 && idx < this->m_iSize;
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
		Expand(m_iCapacity * ms_iExpandingFactor);
	}

	
	/// <summary>
	/// [오버라이딩]
	///  - ArrayQueue
	/// </summary>
	virtual void DestroyAtRange(const int startIdx, const int endIdx) {
		DebugAssertMsg(this->IsValidRange(startIdx, endIdx),
			"올바르지 않은 인덱스 범위(%d ~ %d) 입니다. (%d, 컨테이너 크기: %d)", startIdx, endIdx, this->m_iSize);

		for (int i = startIdx; i <= endIdx; i++) {
			Memory::PlacementDelete<IsPointerType_v<T>>(m_pArray[i]);
		}
	}

	template <typename TPredicate>
	void Sort(TPredicate&& predicate) {
		Arrays::Sort(m_pArray, this->m_iSize, Move(predicate));
	}

	void Sort() {
		Arrays::Sort(m_pArray, this->m_iSize, NaturalOrder{});
	}

	template <typename TPredicate>
	void SortInsertion(TPredicate&& predicate) {
		Arrays::SortInsertion(m_pArray, this->m_iSize, Move(predicate));
	}

	/// <summary>
	/// startIdx 이상 endIdx이하에 위치한 원소들을 정렬한다.
	/// </summary>
	template <typename TPredicate>
	void SortRange(const int startIdx, const int endIdx, TPredicate&& predicate) {
		Arrays::SortRange(m_pArray, startIdx, endIdx, Move(predicate));
	}

	void SortRange(const int startIdx, const int endIdx) {
		Arrays::SortRange(m_pArray, startIdx, endIdx, NaturalOrder{});
	}

protected:
	T& GetAt(const int idx) const {
		DebugAssertMsg(IsValidIndex(idx), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", idx, this->m_iSize);
		return m_pArray[idx];
	}



	void SetAt(const int idx, const T& data) {
		DebugAssertMsg(IsValidIndex(idx), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", idx, this->m_iSize);
		Memory::PlacementNew<IsPointerType_v<T>>(m_pArray[idx]);
		m_pArray[idx] = data;
	}

	/// <summary>
	/// 만든 이유 : ArrayQueue는 Tail이 다음 삽입될 위치인데 삽입시의 위치가 유효한지 체크할 경우
	///            예외를 던지기 때문에..
	/// </summary>
	void SetAtUnsafe(const int idx, const T& data) noexcept {
		Memory::PlacementNew<IsPointerType_v<T>>(m_pArray[idx]);
		m_pArray[idx] = data;
	}

	void SetAtUnsafe(const int idx, T&& data) noexcept {
		Memory::PlacementNew<IsPointerType_v<T>>(m_pArray[idx]);
		m_pArray[idx] = Move(data);
	}

	

	void SetAt(const int idx, T&& data) {
		Memory::PlacementNew<IsPointerType_v<T>>(m_pArray[idx]);
		m_pArray[idx] = Move(data);
	}

	template <typename... Args>
	void EmplaceAt(const int idx, Args&&... args) {
		DebugAssertMsg(IsValidIndex(idx), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", idx, this->m_iSize);

		if constexpr (IsPointerType_v<T>)
			DebugAssertMsg(false, "포인터 타입은 Emplace 기능 사용 금지...");
		else
			Memory::PlacementNew(m_pArray[idx], Forward<Args>(args)...);
	}

	void DestroyAt(const int idx) {
		DebugAssertMsg(IsValidIndex(idx), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", idx, this->m_iSize);

		Memory::PlacementDelete<IsPointerType_v<T>>(m_pArray[idx]);
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
			"(1) 올바르지 않은 데이터 인덱스 입니다. (%d, 컨테이너 크기: %d)", blockIdx, this->m_iSize);
		DebugAssertMsg(IsValidIndex(blockIdx + blockSize - 1), 
			"(2) 올바르지 않은 데이터 인덱스 입니다. (%d, 컨테이너 크기: %d)", blockIdx + blockSize - 1, this->m_iSize);

		// 블록이 이동할 위치가 배열 내부에 둘 수 있는지 체크
		DebugAssertMsg(IsValidIndexCapacity(moveIdx),
			"(3) 올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", moveIdx, this->m_iSize);
		DebugAssertMsg(IsValidIndexCapacity(moveIdx + blockSize - 1),
			"(4) 올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", moveIdx + blockSize - 1, this->m_iSize);

		if (moveIdx > blockIdx) {
			Memory::CopyUnsafeReverse(
				m_pArray + moveIdx,
				m_pArray + blockIdx,
				blockSize * sizeof(T));
			return;
		}

		Memory::CopyUnsafe(
			m_pArray + moveIdx,
			m_pArray + blockIdx,
			blockSize * sizeof(T));
	}


	/// <summary>
	/// 전달받은 사이즈 크기에 맞는 배열 크기를 반환해준다.
	/// </summary>
	int CalculateExpandCapacity(int size) const {
		if (size < m_iCapacity) {
			return m_iCapacity;
		}

		int iExpectedCapacity = m_iCapacity;

		while (true) {
			iExpectedCapacity *= ms_iExpandingFactor;
			if (iExpectedCapacity > size) {
				break;
			}
		}
		return iExpectedCapacity;
	}

	T* Source() {
		return m_pArray;
	}

	CollectionType GetCollectionType() override { return CollectionType::Array; }
protected:
	static constexpr int ms_iExpandingFactor = 4;	// 꽉차면 4배씩 확장
	static constexpr int ms_iDefaultCapacity = 32;	// 초기 배열 크기
protected:
	T* m_pArray;
	int m_iCapacity;

	friend class TArrayCollectionIterator;
};

template <typename T, typename TAllocator>
ArrayCollection<T, TAllocator>::~ArrayCollection() noexcept {}

NS_JC_END