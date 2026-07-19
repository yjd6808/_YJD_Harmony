/*
 * 작성자 : 윤정도
 */


#pragma once

#include "jc/Memory.h"

NS_JC_BEGIN

template <typename T, int VCapacity>
class StaticVector
{
public:
    StaticVector()
        : size_(0)
    {
    }

    StaticVector(std::initializer_list<T> _ilist)
        : size_(0)
    {
        for (const T& element : _ilist)
            Add(element);
    }

    StaticVector(const StaticVector& _other)
        : size_(0)
    {
        CopyFrom(_other);
    }

    StaticVector(StaticVector&& _other) noexcept
        : size_(0)
    {
        CopyFrom(Move(_other));
    }

    ~StaticVector() noexcept
    {
        Clear();
    }

    StaticVector& operator=(const StaticVector& _other)
    {
        CopyFrom(_other);
        return *this;
    }

    StaticVector& operator=(StaticVector&& _other) noexcept
    {
        CopyFrom(Move(_other));
        return *this;
    }

    StaticVector& operator=(std::initializer_list<T> _ilist)
    {
        Clear();
        for (const T& element : _ilist)
            Add(element);
        return *this;
    }

    bool Add(const T& _value)
    {
        if (size_ >= VCapacity)
            return false;

        ConstructAt(size_++, _value);
        return true;
    }

    bool Add(T&& _value)
    {
        if (size_ >= VCapacity)
            return false;

        ConstructAt(size_++, Move(_value));
        return true;
    }

    bool RemoveAt(int _index)
    {
        if (_index < 0 || _index >= size_)
            return false;

        DestroyAt(_index);
        for (int i = _index; i < size_ - 1; ++i)
            ConstructAt(i, Move(data_[i + 1]));

        DestroyAt(size_ - 1);
        size_--;
        return true;
    }

    bool Remove(const T& _value)
    {
        int offset = Offset(_value);
        if (offset == -1)
            return false;

        RemoveAt(offset);
        return true;
    }

    void Clear()
    {
        if constexpr (!IsPointerType_v<T>)
        {
            for (int i = 0; i < size_; ++i)
                Memory::PlacementDelete(data_[i]);
        }
        size_ = 0;
    }

    T& operator[](int _index)
    {
        jc_assert_msg(_index >= 0 && _index < size_, "StaticVector: index out of range. (%d, size: %d)", _index, size_);
        return data_[_index];
    }

    const T& operator[](int _index) const
    {
        jc_assert_msg(_index >= 0 && _index < size_, "StaticVector: index out of range. (%d, size: %d)", _index, size_);
        return data_[_index];
    }

    int Size() const { return size_; }
    static constexpr int GetCapacity() { return VCapacity; }
    bool IsEmpty() const { return size_ == 0; }
    bool IsFull() const { return size_ == VCapacity; }

    int Offset(const T& _value) const
    {
        for (int i = 0; i < size_; ++i)
            if (data_[i] == _value)
                return i;
        return -1;
    }

    bool Exist(const T& _value) const
    {
        return Offset(_value) >= 0;
    }

    template <typename TPredicate>
    bool ExistIf(TPredicate&& _predicate) const
    {
        for (int i = 0; i < size_; ++i)
            if (_predicate(data_[i]))
                return true;
        return false;
    }

    template <typename TPredicate>
    T* FindIf(TPredicate&& _predicate)
    {
        for (int i = 0; i < size_; ++i)
            if (_predicate(data_[i]))
                return &data_[i];
        return nullptr;
    }

    template <typename TPredicate>
    const T* FindIf(TPredicate&& _predicate) const
    {
        for (int i = 0; i < size_; ++i)
            if (_predicate(data_[i]))
                return &data_[i];
        return nullptr;
    }

    T* Source() { return data_; }
    const T* Source() const { return data_; }

    struct SIterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        SIterator(T* _pData) : pData_(_pData) {}

        reference operator*() const { return *pData_; }
        pointer operator->() { return pData_; }
        SIterator& operator++() { pData_++; return *this; }
        SIterator operator++(int) { SIterator tmp = *this; ++(*this); return tmp; }
        friend bool operator==(const SIterator& _lhs, const SIterator& _rhs) { return _lhs.pData_ == _rhs.pData_; }
        friend bool operator!=(const SIterator& _lhs, const SIterator& _rhs) { return _lhs.pData_ != _rhs.pData_; }

    private:
        T* pData_;
    };

    struct CConstSIterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;

        CConstSIterator(const T* _pData) : pData_(_pData) {}

        reference operator*() const { return *pData_; }
        pointer operator->() { return pData_; }
        CConstSIterator& operator++() { pData_++; return *this; }
        CConstSIterator operator++(int) { CConstSIterator tmp = *this; ++(*this); return tmp; }
        friend bool operator==(const CConstSIterator& _lhs, const CConstSIterator& _rhs) { return _lhs.pData_ == _rhs.pData_; }
        friend bool operator!=(const CConstSIterator& _lhs, const CConstSIterator& _rhs) { return _lhs.pData_ != _rhs.pData_; }

    private:
        const T* pData_;
    };

    SIterator begin() { return SIterator(data_); }
    SIterator end() { return SIterator(data_ + size_); }
    CConstSIterator begin() const { return CConstSIterator(data_); }
    CConstSIterator end() const { return CConstSIterator(data_ + size_); }

private:
    void CopyFrom(const StaticVector& _other)
    {
        Clear();
        for (int i = 0; i < _other.size_; ++i)
            ConstructAt(i, _other.data_[i]);
        size_ = _other.size_;
    }

    void CopyFrom(StaticVector&& _other)
    {
        Clear();
        for (int i = 0; i < _other.size_; ++i)
            ConstructAt(i, Move(_other.data_[i]));
        size_ = _other.size_;
        _other.Clear();
    }

    template <typename Ty>
    void ConstructAt(int _index, Ty&& _value)
    {
        if constexpr (IsPointerType_v<T>)
        {
            data_[_index] = Forward<Ty>(_value);
        }
        else
        {
            Memory::PlacementNew(data_[_index], Forward<Ty>(_value));
        }
    }

    void DestroyAt(int _index)
    {
        if constexpr (!IsPointerType_v<T>)
            Memory::PlacementDelete(data_[_index]);
    }

    int size_;
    T data_[VCapacity];
};

NS_END
