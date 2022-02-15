/*
	작성자 : 윤정도
	배열 기반 컨테이너의 추상화 클래스
*/

#pragma once

#include <JCore/Memory.h>

#include <JCore/Container/Arrays.h>
#include <JCore/Container/Collection.h>
#include <JCore/Container/ArrayCollectionIterator.h>


namespace JCore {


/*=====================================================================================
									다이나믹 배열
					배열 스택, 배열 큐, 배열 리스트의 공통 인터페이스 정의
=====================================================================================*/

template <typename T>
class ArrayCollection : public Collection<T>
{
	using TCollection				= typename Collection<T>;
	using TArrayCollection			= typename ArrayCollection<T>;
	using TArrayCollectionIterator  = typename ArrayCollectionIterator<T>;
public:
	ArrayCollection(int capacity) : TCollection()  {
		if (capacity < 1) {
			throw InvalidArgumentException("컨테이너의 크기가 0이하가 될 수 없습니다.");
		}

		m_pArray = Memory::Allocate<T*>(sizeof(T) * capacity);
		m_iCapacity = capacity;
	}

	virtual ~ArrayCollection() noexcept = 0;

	int Capacity() const {
		return m_iCapacity;
	}

	/// <summary>
	/// 내부 원소 모두 제거
	/// 
	/// [오버라이딩]
	///  - ArrayQueue
	/// </summary>
	virtual void Clear() {
		DestroyAtRange(0, this->Size() - 1);
		this->m_iSize = 0;
	}

	/// <summary>
	/// [오버라이딩]
	///  - ArrayQueue : 한칸이 덜 찬 상태를 꽉찬 상태로 처리해야하기 때문에 오버라이딩 해야함
	/// </summary>
	virtual bool IsFull() const {
		return this->m_iSize == m_iCapacity;
	}
protected:
	
	// 용량 수정
	virtual void Resize(int capacity) {
		if (capacity >= this->m_iSize) {
			Expand(capacity);
			return;
		} 

		DestroyAtRange(m_iCapacity, this->m_iSize - 1);
		m_iCapacity = capacity;
	}

	
	/// <summary>
	/// 현재 용량보다 더 큰 용량으로 확장
	/// 
	/// [오버라이딩]
	/// - ArrayQueue
	/// </summary>
	/// <param name="newCapacity">기존 용량보다 더 큰 값</param>
	virtual void Expand(int newCapacity) {
		ThrowIfNewCapacityIsSmallerThanBefore(newCapacity);
		T* pNewArray = Memory::Allocate<T*>(sizeof(T) * newCapacity);
		Memory::Copy(pNewArray, sizeof(T) * newCapacity, m_pArray, sizeof(T) * this->m_iSize);
		Memory::Deallocate(m_pArray);
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
		ThrowIfRangeIsInvalid(startIdx, endIdx);

		for (int i = startIdx; i <= endIdx; i++) {
			Memory::PlacementDeallocate(m_pArray[i]);
		}
	}


	

protected:
	T& GetAt(const int idx) const {
		ThrowIfIndexIsInvalid(idx);
		return m_pArray[idx];
	}

	void SetAt(const int idx, const T& data) {
		ThrowIfIndexIsInvalid(idx);
		Memory::PlacementAllocate(m_pArray[idx]);
		m_pArray[idx] = data;
	}

	/// <summary>
	/// 만든 이유 : ArrayQueue는 Tail이 다음 삽입될 위치인데 삽입시의 위치가 유효한지 체크할 경우
	///            예외를 던지기 때문에..
	/// </summary>
	void SetAtUnsafe(const int idx, const T& data) noexcept {
		Memory::PlacementAllocate(m_pArray[idx]);
		m_pArray[idx] = data;
	}

	void SetAtUnsafe(const int idx, T&& data) noexcept {
		Memory::PlacementAllocate(m_pArray[idx]);
		m_pArray[idx] = Move(data);
	}

	

	void SetAt(const int idx, T&& data) {
		Memory::PlacementAllocate(m_pArray[idx]);
		m_pArray[idx] = Move(data);
	}

	template <typename... Args>
	void EmplaceAt(const int idx, Args&&... args) {
		ThrowIfIndexIsInvalid(idx);
		Memory::PlacementAllocate(m_pArray[idx], Forward<Args>(args)...);
	}

	void DestroyAt(const int idx) {
		ThrowIfIndexIsInvalid(idx);
		Memory::PlacementDeallocate(m_pArray[idx]);
	}

	/// <summary>
	/// 블록을 원하는 위치로 이동한다.
	/// 블록은 배열의 특정 인덱스부터 정해진 갯수까지의 구간을 블록이라 한다.
	/// </summary>
	/// <param name="blockIdx"> 블록 시작 위치 </param>
	/// <param name="blockSize"> 블록 크기 </param>
	/// <param name="moveCount"> 이동할 위치 </param>
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
		ThrowIfIndexIsInvalid(blockIdx);
		ThrowIfIndexIsInvalid(blockIdx + blockSize - 1);

		// 블록이 이동할 위치가 배열 내부에 둘 수 있는지 체크
		ThrowIfIndexIsNotCapacityIndex(moveIdx);
		ThrowIfIndexIsNotCapacityIndex(moveIdx + blockSize - 1);

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
	int CalculateExpandCapacity(int size) {
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
protected:
	virtual void ThrowIfContainerIsEmpty() const {
		if (this->IsEmpty()) {
			throw InvalidOperationException("데이터가 존재하지 않습니다.");
		}
	}

	virtual void ThrowIfIndexIsInvalid(int idx) const {
		if (!IsValidIndex(idx)) {
			throw OutOfRangeException("올바르지 않은 데이터 인덱스 입니다.");
		}
	}

	virtual void ThrowIfIndexIsNotCapacityIndex(int capacityIdx) const {
		if (!IsValidIndexCapacity(capacityIdx)) {
			throw OutOfRangeException("올바르지 않은 배열 인덱스 입니다.");
		}
	}

	virtual void ThrowIfRangeIsInvalid(int startIdx, int endIdx) const {
		if (!IsValidRange(startIdx, endIdx)) {
			throw OutOfRangeException("올바르지 않은 인덱스 범위입니다.");
		}
	}

	virtual void ThrowIfNewCapacityIsSmallerThanBefore(int newCapacity) const {
		if (newCapacity < m_iCapacity) {
			throw InvalidArgumentException("현재 용량보다 더 작은 용량입니다.");
		}
	}
protected:
	static constexpr int ms_iExpandingFactor = 4;	// 꽉차면 4배씩 확장
	static constexpr int ms_iDefaultCapcity = 32;	// 초기 배열 크기
protected:
	T* m_pArray;
	int m_iCapacity;

	friend class TArrayCollectionIterator;
};

template <typename T>
ArrayCollection<T>::~ArrayCollection() noexcept {
	Memory::Deallocate(m_pArray);
}

} // namespace JCore