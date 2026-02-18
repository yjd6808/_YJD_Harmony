/*
    작성자 : 윤정도
*/

#pragma once

#include <jc/Container/ArrayCollection.h>
#include <jc/Container/VectorIterator.h>

NS_JC_BEGIN

/*=====================================================================================
                                  벡터
=====================================================================================*/

template <typename T, typename TAllocator = CDefaultAllocator>
class Vector : public ArrayCollection<T, TAllocator>
{
    // static_assert(IsValidAllocator_v<T, TAllocator>, "... wrong allocator syntax !!");

    using TEnumerator = Enumerator<T, TAllocator>;
    using TCollection = Collection<T, TAllocator>;
    using TArrayCollection = ArrayCollection<T, TAllocator>;
    using TVector = Vector<T, TAllocator>;
    using TVectorIterator = VectorIterator<T, TAllocator>;

  public:
    Vector(int _capacity = TArrayCollection::DEFAULT_CAPACITY) : TArrayCollection(_capacity)
    {
    }

    Vector(int _size, const T& _initData) : TArrayCollection(_size, _initData)
    {
    }

    Vector(int _size, T&& _initData) : TArrayCollection(_size, Move(_initData))
    {
    }

    Vector(const TVector& _other) : TArrayCollection(_other)
    {
    }

    Vector(TVector&& _other) noexcept : TArrayCollection(Move(_other))
    {
    }

    Vector(std::initializer_list<T> _ilist) : TArrayCollection(_ilist)
    {
    }

    ~Vector() noexcept override
    {
        this->Clear(true);
    }

  public:
    TVector& operator=(const TVector& _other)
    {
        this->CopyFrom(_other);
        return *this;
    }

    TVector& operator=(TVector&& _other) noexcept
    {
        this->CopyFrom(Move(_other));
        return *this;
    }

    TVector& operator=(std::initializer_list<T> _ilist)
    {
        this->CopyFrom(_ilist);
        return *this;
    }

    T& At(const int _idx) const
    {
        return this->GetAt(_idx);
    }

    void PushBack(const T& _data)
    {
        if(this->IsFull())
        {
            this->ExpandAuto();
        }

        this->SetAt(this->size_++, _data);
    }

    template <Int32U Size>
    void PushBack(const T (&_data)[Size])
    {
        this->ExpandIfNeeded(this->Size() + Size);

        for(int i = 0; i < Size; ++i)
        {
            this->SetAt(this->size_++, _data[i]);
        }
    }

    void PushBack(T* _pData, int _count)
    {
        this->ExpandIfNeeded(this->Size() + _count);

        for(int i = 0; i < _count; ++i)
        {
            this->SetAt(this->size_++, _pData[i]);
        }
    }

    void PushBack(T&& _data)
    {
        if(this->IsFull())
        {
            this->ExpandAuto();
        }

        this->SetAt(this->size_++, Move(_data));
    }

    T& Front()
    {
        if(this->size_ == 0)
        {
            throw InvalidOperationException{"데이터가 없습니다."};
        }

        return this->pArray_[0];
    }

    T& Back()
    {
        if(this->size_ == 0)
        {
            throw InvalidOperationException{"데이터가 없습니다."};
        }

        return this->pArray_[this->size_ - 1];
    }

    /**
     * \brief 용량을 수정하도록 한다. 만약 기존에 담긴 데이터 수가 전달해준
     * 용량보다 많을 경우 소멸자를 호출하여 넘치는 만큼 삭제해줌
     */
    void Resize(int _capacity)
    {
        TArrayCollection::Resize(_capacity, T{});
    }

    void Resize(int _capacity, const T& _val)
    {
        TArrayCollection::Resize(_capacity, _val);
    }

    void Reserve(int _capacity)
    {
        TArrayCollection::Expand(_capacity);
    }

    void Shrink(int _capacity) override
    {
        return TArrayCollection::Shrink(_capacity);
    }

    void ShrinkToFit(float _ratio = 1.0f) override
    {
        return TArrayCollection::ShrinkToFit(_ratio);
    }

    /// <summary>>a
    /// 여러개의 데이터를 뒤에 추가하고자할 때
    /// </summary>
    void PushBackAll(const TCollection& _collection)
    {
        int expandSize = this->CalculateExpandCapacity(this->size_ + _collection.Size());

        if(expandSize > this->capacity_)
        {
            this->Expand(expandSize);
        }

        TEnumerator it = _collection.Begin();
        while(it->HasNext())
        {
            this->SetAt(this->size_++, it->Next());
        }
    }

    void Insert(int _idx, const T& _data)
    {
        // 마지막 위치에 삽입하는 경우 그냥 PushBack을 수행하자.
        if(_idx == this->size_)
        {
            PushBack(_data);
        }

        if(this->IsFull())
        {
            this->ExpandAuto();
        }

        int moveBlockSize = this->size_ - _idx;

        this->MoveBlock(_idx, _idx + 1, moveBlockSize);
        this->SetAt(_idx, _data);
        ++this->size_;
    }

    void Insert(int _idx, T&& _data)
    {
        // 마지막 위치에 삽입하는 경우 그냥 PushBack을 수행하자.
        if(_idx == this->size_)
        {
            PushBack(Move(_data));
            return;
        }

        if(this->IsFull())
        {
            this->ExpandAuto();
        }

        int moveBlockSize = this->size_ - _idx;

        this->MoveBlock(_idx, _idx + 1, moveBlockSize);
        this->SetAt(_idx, Move(_data));
        ++this->size_;
    }

    /// <summary>
    /// 특정 위치에 데이터를 많이 삽입하고자 할 때
    /// </summary>
    void InsertAll(int _idx, const TCollection& _collection)
    {
        if(_idx == this->size_)
        {
            PushBackAll(_collection);
            return;
        }

        int collectionSize = _collection.Size();
        int expandSize = this->CalculateExpandCapacity(this->size_ + collectionSize);

        if(expandSize > this->capacity_)
        {
            this->Expand(expandSize);
        }

        int moveBlockSize = this->size_ - _idx;

        this->MoveBlock(_idx, _idx + collectionSize, moveBlockSize);

        TEnumerator it = _collection.Begin();
        while(it->HasNext())
        {
            this->SetAtUnsafe(_idx++, it->Next());
        }

        this->size_ += collectionSize;
    }

    /// <summary>
    /// 인자를 받아서 내부에서 생성한다.
    /// </summary>
    template <typename... Args>
    void EmplaceBack(Args&&... _args)
    {
        if(this->IsFull())
        {
            this->ExpandAuto();
        }

        this->EmplaceAt(this->size_++, Forward<Args>(_args)...);
    }

    /// <summary>
    /// 특정 위치에 컨테이너 내부에서 생성한다.
    /// </summary>
    template <typename... Args>
    void EmplaceInsert(int _idx, Args&&... _args)
    {
        if(_idx == this->size_)
        {
            EmplaceBack(Forward<Args>(_args)...);
            return;
        }

        if(this->IsFull())
        {
            this->ExpandAuto();
        }

        this->MoveBlock(_idx, _idx + 1, this->size_ - _idx);
        this->EmplaceAt(_idx, Forward<Args>(_args)...);
        ++this->size_;
    }

    /// <summary>
    /// 선향탐색으로 검색하여 데이터가 존재하는지 확인한다.
    /// </summary>
    bool Exist(const T& _data) const
    {
        return Offset(_data) >= 0;
    }

    /// <summary>
    /// 이진탐색으로 검색하여 데이터가 존재하는지 확인한다.
    /// 단, 정렬된 상태여야 정상적으로 동작한다.
    /// </summary>
    bool ExistBinary(const T& _data) const
    {
        return OffsetBinary(_data) >= 0;
    }

    /// <summary>
    /// 선향탐색으로 검색하여 데이터가 처음으로 시작되는 위치(인덱스)를 반환한다.
    /// </summary>
    int Offset(const T& _data) const
    {
        return Arrays::LinearSearch(this->pArray_, this->size_, _data);
    }

    /// <summary>
    /// 이분탐색으로 검색하여 데이터가 처음으로 시작되는 위치(인덱스)를 반환한다.
    /// 단, 정렬된 상태여야 정상적으로 동작한다.
    /// </summary>
    int OffsetBinary(const T& _data) const
    {
        int offset = Arrays::LowerBound(this->pArray_, this->size_, _data);

        if(offset >= this->size_)
        {
            return -1;
        }

        if(this->pArray_[offset] != _data)
        {
            return -1;
        }

        return offset;
    }

    /**
     * \brief 사용방법은 Arrays::LowerBound 주석 참고
     */
    template <typename TVal, typename TPredicate>
    int OffsetLowerBound(TVal&& _data, TPredicate&& _predicate) const
    {
        int offset = Arrays::LowerBound(this->pArray_, this->size_, Forward<TVal>(_data), Forward<TPredicate>(_predicate));

        if(offset >= this->size_)
        {
            return -1;
        }

        return offset;
    }

    /// <summary>
    /// 선형탐색으로 데이터를 검색하여 삭제한다.
    /// </summary>
    bool Remove(const T& _data)
    {
        int offset = Offset(_data);

        if(offset == -1)
        {
            return false;
        }

        int moveBlockSize = this->size_ - (offset + 1);

        this->DestroyAt(offset);
        this->MoveBlock(offset + 1, offset, moveBlockSize);

        --this->size_;
        return true;
    }

    /// <summary>
    /// 이진탐색으로 데이터를 검색하여 삭제한다.
    /// </summary>
    bool RemoveBinary(const T& _data)
    {
        int offset = OffsetBinary(_data);

        if(offset == -1)
        {
            return false;
        }

        RemoveAt(offset);
        return true;
    }

    /// <summary>
    /// 특정 인덱스의 데이터를 삭제한다.
    /// </summary>
    void RemoveAt(const int _idx)
    {
        jc_assert_msg(this->IsValidIndex(_idx), "올바르지 않은 데이터 인덱스(%d) 입니다. (컨테이너 크기: %d)", _idx, this->size_);

        const int moveBlockSize = this->size_ - (_idx + 1);

        this->DestroyAt(_idx);
        this->MoveBlock(_idx + 1, _idx, moveBlockSize);

        --this->size_;
    }

    template <typename Predicate>
    bool RemoveIf(Predicate&& _predicate)
    {
        int removeIndex = -1;
        for(int i = 0; i < this->size_; ++i)
        {
            if(_predicate(this->pArray_[i]))
            {
                removeIndex = i;
                break;
            }
        }
        if(removeIndex != -1)
        {
            RemoveAt(removeIndex);
            return true;
        }

        return false;
    }

    void PopBack()
    {
        RemoveAt(this->size_ - 1);
    }

    void PopFront(int _count = 1)
    {
        if(_count <= 0)
        {
            return;
        }

        if(_count > this->size_)
        {
            _count = this->size_;
        }

        const int moveBlockSize = this->size_ - _count;
        this->DestroyAtRange(0, _count - 1);
        this->MoveBlock(_count, 0, moveBlockSize);
        this->size_ -= _count;
    }

    void Sort()
    {
        TArrayCollection::Sort(NaturalOrder{});
    }

    template <typename TPredicate>
    void Sort(TPredicate&& _predicate)
    {
        TArrayCollection::Sort(Forward<TPredicate>(_predicate));
    }

    void SortRange(const int _startIdx, const int _endIdx)
    {
        TArrayCollection::SortRange(_startIdx, _endIdx, NaturalOrder{});
    }

    template <typename TPredicate>
    void SortRange(const int _startIdx, const int _endIdx, TPredicate&& _predicate)
    {
        TArrayCollection::SortRange(_startIdx, _endIdx, Forward<TPredicate>(_predicate));
    }

    template <typename TPredicate>
    void InsertionSort(TPredicate&& _predicate)
    {
        TArrayCollection::InsertionSort(Forward<TPredicate>(_predicate));
    }

    T& operator[](const int _idx) const
    {
        return this->GetAt(_idx);
    }

    TEnumerator Begin() const override
    {
        return MakeShared<TVectorIterator, TAllocator>(this->GetOwner(), 0);
    }

    TEnumerator End() const override
    {
        return MakeShared<TVectorIterator, TAllocator>(this->GetOwner(), this->Size());
    }

    template <typename Consumer>
    void ForEach(Consumer&& _consumer)
    {
        for(int i = 0; i < this->size_; ++i)
        {
            _consumer(this->pArray_[i]);
        }
    }

    void ForEachDelete()
    {
        if constexpr(!IsPointerType_v<T>)
        {
            jc_assert(false);
            return;
        }

        for(int i = 0; i < this->size_; ++i)
        {
            delete this->pArray_[i];
        }
    }

    template <typename Predicate>
    T* FindIf(Predicate&& _predicate)
    {
        for(int i = 0; i < this->size_; ++i)
        {
            if(_predicate(this->pArray_[i]))
            {
                return &this->pArray_[i];
            }
        }
        return nullptr;
    }

    ContainerType GetContainerType() override
    {
        return ContainerType::Vector;
    }

	struct SIterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;

		SIterator(const TEnumerator& enumerator) : m_it(enumerator) {}

		reference operator*() const { return *m_it->Current(); }
		pointer operator->() { return &m_it->Current(); }
		SIterator& operator++() { m_it->Next(); return *this; }
		SIterator operator++(int) { SIterator tmp = *this; ++(*this); return tmp; }
		friend bool operator== (const SIterator& a, const SIterator& b) { return a.m_it == b.m_it; }
		friend bool operator!= (const SIterator& a, const SIterator& b) { return a.m_it == b.m_it; }
	private:
		TEnumerator m_it;
	};

    SIterator begin()
    {
        return SIterator(Begin());
    }
    SIterator end()
    {
        return SIterator(End());
    }

  protected:
    friend class TVectorIterator;
};

NS_END
