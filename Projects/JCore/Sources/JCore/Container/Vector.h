/*
	작성자 : 윤정도
*/

#pragma once


#include <JCore/Container/VectorIterator.h>
#include <JCore/Container/ArrayCollection.h>

NS_JC_BEGIN

/*=====================================================================================
								  벡터
=====================================================================================*/


template <typename T, typename TAllocator = DefaultAllocator>
class Vector : public ArrayCollection<T, TAllocator>
{
	//static_assert(IsValidAllocator_v<T, TAllocator>, "... wrong allocator syntax !!");

	using TEnumerator			= Enumerator<T, TAllocator>;
	using TCollection			= Collection<T, TAllocator>;
	using TArrayCollection		= ArrayCollection<T, TAllocator>;
	using TVector				= Vector<T, TAllocator>;
	using TVectorIterator		= VectorIterator<T, TAllocator>;
public:
	Vector(int capacity = TArrayCollection::ms_iDefaultCapacity) : TArrayCollection(capacity) {}

	Vector(int size, const T& initData) : TArrayCollection(size, initData) {}

	Vector(int size, T&& initData) : TArrayCollection(size, Move(initData)) {}

	Vector(const TVector& other) : TArrayCollection(other) {}

	Vector(TVector&& other) noexcept : TArrayCollection(Move(other)) {}

	Vector(std::initializer_list<T> ilist) : TArrayCollection(ilist) {}

	~Vector() noexcept override {
		this->Clear(true);
	}

public:
	TVector& operator=(const TVector& other) {
		this->CopyFrom(other);
		return *this;
	}

	TVector& operator=(TVector&& other) noexcept {
		this->CopyFrom(Move(other));
		return *this;
	}

	TVector& operator=(std::initializer_list<T> ilist) {
		this->CopyFrom(ilist);
		return *this;
	}

	T& At(const int idx) {
		return this->GetAt(idx);
	}

	void PushBack(const T& data) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->SetAtUnsafe(this->m_iSize++, data);
	}

	template <Int32U Size>
	void PushBack(const T(&data)[Size]) {
		this->ExpandIfNeeded(this->Size() + Size);

		for (int i = 0; i < Size; ++i) {
			this->SetAtUnsafe(this->m_iSize++, data[i]);
		}
	}

	void PushBack(T* data, int count) {
		this->ExpandIfNeeded(this->Size() + count);

		for (int i = 0; i < count; ++i) {
			this->SetAtUnsafe(this->m_iSize++, data[i]);
		}
	}

	void PushBack(T&& data) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->SetAtUnsafe(this->m_iSize++, Move(data));
	}

	/**
	 * \brief 용량을 수정하도록 한다. 만약 기존에 담긴 데이터 수가 전달해준
	 * 용량보다 많을 경우 소멸자를 호출하여 넘치는 만큼 삭제해줌
	 */
	void Resize(int capacity) override {
		return TArrayCollection::Resize(capacity);
	}

	/// <summary>>a
	/// 여러개의 데이터를 뒤에 추가하고자할 때
	/// </summary>
	void PushBackAll(const TCollection& collection) {
		int iExpandSize = this->CalculateExpandCapacity(this->m_iSize + collection.Size());

		if (iExpandSize > this->m_iCapacity) {
			this->Expand(iExpandSize);
		}

		TEnumerator it = collection.Begin();
		while (it->HasNext()) {
			this->SetAtUnsafe(this->m_iSize++, it->Next());
		}
	}

	void Insert(int idx, const T& data) {
		// 마지막 위치에 삽입하는 경우 그냥 PushBack을 수행하자.
		if (idx == this->m_iSize) {
			PushBack(data);
		}

		if (this->IsFull()) {
			this->ExpandAuto();
		}

		int iMoveBlockSize = this->m_iSize - idx;
		
		this->MoveBlock(
			idx, 
			idx + 1, 
			iMoveBlockSize);
		this->SetAt(idx, data);
		++this->m_iSize;
	}

	void Insert(int idx, T&& data) {
		// 마지막 위치에 삽입하는 경우 그냥 PushBack을 수행하자.
		if (idx == this->m_iSize) {
			PushBack(Move(data));
			return;
		}

		if (this->IsFull()) {
			this->ExpandAuto();
		}

		int iMoveBlockSize = this->m_iSize - idx;

		this->MoveBlock(
			idx,
			idx + 1,
			iMoveBlockSize);
		this->SetAt(idx, Move(data));
		++this->m_iSize;
	}

	/// <summary>
	/// 특정 위치에 데이터를 많이 삽입하고자 할 때
	/// </summary>
	void InsertAll(int idx, const TCollection& collection) {
		if (idx == this->m_iSize) {
			PushBackAll(collection);
			return;
		}

		int iCollectionSize = collection.Size();
		int iExpandSize = this->CalculateExpandCapacity(this->m_iSize + iCollectionSize);

		if (iExpandSize > this->m_iCapacity) {
			this->Expand(iExpandSize);
		}

		int iMoveBlockSize = this->m_iSize - idx;

		this->MoveBlock(
			idx,
			idx + iCollectionSize,
			iMoveBlockSize);

		TEnumerator it = collection.Begin();
		while (it->HasNext()) {
			this->SetAtUnsafe(idx++, it->Next());
		}

		this->m_iSize += iCollectionSize;
	}


	/// <summary>
	/// 인자를 받아서 내부에서 생성한다.
	/// </summary>
	template <typename... Args>
	void EmplaceBack(Args&&... args) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->EmplaceAt(this->m_iSize++, Forward<Args>(args)...);
	}

	/// <summary>
	/// 특정 위치에 컨테이너 내부에서 생성한다.
	/// </summary>
	template <typename... Args>
	void EmplaceInsert(int idx, Args&&... args) {
		if (idx == this->m_iSize) {
			EmplaceBack(Forward<Args>(args)...);
			return;
		}

		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->MoveBlock(
			idx,
			idx + 1,
			this->m_iSize - idx);
		this->EmplaceAt(idx, Forward<Args>(args)...);
		++this->m_iSize;
	}

	/// <summary>
	/// 선향탐색으로 검색하여 데이터가 존재하는지 확인한다.
	/// </summary>
	bool Exist(const T& data) const {
		return Offset(data) >= 0;
	}

	/// <summary>
	/// 이진탐색으로 검색하여 데이터가 존재하는지 확인한다.
	/// 단, 정렬된 상태여야 정상적으로 동작한다.
	/// </summary>
	bool ExistBinary(const T& data) const {
		return OffsetBinary(data) >= 0;
	}

	/// <summary>
	/// 선향탐색으로 검색하여 데이터가 처음으로 시작되는 위치(인덱스)를 반환한다.
	/// </summary>
	int Offset(const T& data) const {
		return Arrays::LinearSearch(this->m_pArray, this->m_iSize, data);
	}

	/// <summary>
	/// 이분탐색으로 검색하여 데이터가 처음으로 시작되는 위치(인덱스)를 반환한다.
	/// 단, 정렬된 상태여야 정상적으로 동작한다.
	/// </summary>
	int OffsetBinary(const T& data) const {
		int iOffset = Arrays::LowerBound(this->m_pArray, this->m_iSize, data);

		if (iOffset >= this->m_iSize) {
			return -1;
		}

		if (this->m_pArray[iOffset] != data) {
			return -1;
		}

		return iOffset;
	}

	/**
	 * \brief 사용방법은 Arrays::LowerBound 주석 참고
	 */
	template <typename TVal, typename TPredicate>
	int OffsetLowerBound(TVal&& data, TPredicate&& predicate) const {
		int iOffset = Arrays::LowerBound(this->m_pArray, this->m_iSize, Forward<TVal>(data), Forward<TPredicate>(predicate));

		if (iOffset >= this->m_iSize) {
			return -1;
		}

		return iOffset;
	}

	/// <summary>
	/// 선형탐색으로 데이터를 검색하여 삭제한다.
	/// </summary>
	bool Remove(const T& data) {
		int iOffset = Offset(data);

		if (iOffset == -1) {
			return false;
		}

		int iMoveBlockSize = this->m_iSize - (iOffset + 1);

		this->DestroyAt(iOffset);
		this->MoveBlock(
			iOffset + 1,
			iOffset,
			iMoveBlockSize);

		--this->m_iSize;
		return true;
	}

	/// <summary>
	/// 이진탐색으로 데이터를 검색하여 삭제한다.
	/// </summary>
	bool RemoveBinary(const T& data) {
		int iOffset = OffsetBinary(data);

		if (iOffset == -1) {
			return false;
		}

		RemoveAt(iOffset);
		return true;
	}

	

	/// <summary>
	/// 특정 인덱스의 데이터를 삭제한다.
	/// </summary>
	void RemoveAt(const int idx) {
		DebugAssertMsg(this->IsValidIndex(idx), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", idx, this->m_iSize);

		int iMoveBlockSize = this->m_iSize - (idx + 1);

		this->DestroyAt(idx);
		this->MoveBlock(
			idx + 1,
			idx,
			iMoveBlockSize);

		--this->m_iSize;
	}

	template <typename Predicate>
	bool RemoveIf(Predicate&& predicate) {
		int iRemoveIndex = -1;
		for (int i = 0; i < this->m_iSize; ++i) {
			if (predicate(this->m_pArray[i])) {
				iRemoveIndex = i;
				break;
			}
		}
		if (iRemoveIndex != -1) {
			RemoveAt(iRemoveIndex);
			return true;
		}

		return false;
	}

	void Sort() {
		TArrayCollection::Sort(NaturalOrder{});
	}

	template <typename TPredicate>
	void Sort(TPredicate&& predicate) {
		TArrayCollection::Sort(Move(predicate));
	}

	void SortRange(const int startIdx, const int endIdx) {
		TArrayCollection::SortRange(startIdx, endIdx, NaturalOrder{});
	}

	template <typename TPredicate>
	void SortRange(const int startIdx, const int endIdx, TPredicate&& predicate) {
		TArrayCollection::SortRange(startIdx, endIdx, Move(predicate));
	}

	template <typename TPredicate>
	void SortInsertion(TPredicate&& predicate) {
		TArrayCollection::SortInsertion(Move(predicate));
	}

	T& operator[](const int idx) const {
		return this->GetAt(idx);
	}

	TEnumerator Begin() const override {
		return MakeShared<TVectorIterator, TAllocator>(this->GetOwner(), 0);
	}

	TEnumerator End() const override {
		return MakeShared<TVectorIterator, TAllocator>(this->GetOwner(), this->Size());
	}

	template <typename Consumer>
	void ForEach(Consumer&& consumer) {
		for (int i = 0; i < this->m_iSize; ++i) {
			consumer(this->m_pArray[i]);
		}
	}

	

	ContainerType GetContainerType() override { return ContainerType::Vector; }

	struct Iterator 
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

        Iterator(const TEnumerator& enumerator) : m_it(enumerator) {}

        reference operator*() const { return *m_it->Current(); }
        pointer operator->() { return &m_it->Current(); }
        Iterator& operator++() { m_it->Next(); return *this; }  
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
        friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_it == b.m_it; }
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_it == b.m_it; }
    private:
		TEnumerator m_it;
    };

	Iterator begin() { return Iterator(Begin()); }
	Iterator end() { return Iterator(End()); }

protected:
	friend class TVectorIterator;
};

NS_JC_END