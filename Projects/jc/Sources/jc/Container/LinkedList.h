/*
	작성자 : 윤정도
*/

#pragma once

#include <jc/Container/ListCollection.h>
#include <jc/Container/LinkedListIterator.h>

NS_JC_BEGIN

template <typename T, typename TAllocator = CDefaultAllocator>
class LinkedList : public ListCollection<T, TAllocator>
{
    using TListNode = ListNode<T, TAllocator>;
    using TEnumerator = Enumerator<T, TAllocator>;
    using TCollection = Collection<T, TAllocator>;
    using TListCollection = ListCollection<T, TAllocator>;
    using TLinkedList = LinkedList<T, TAllocator>;
    using TLinkedListIterator = LinkedListIterator<T, TAllocator>;

public:
    LinkedList()
        : TListCollection()
    {
    }

    LinkedList(const TLinkedList& _other)
        : TListCollection(_other)
    {
    }

    LinkedList(TLinkedList&& _other) noexcept
        : TListCollection(Move(_other))
    {
    }

    LinkedList(std::initializer_list<T> _ilist)
        : TListCollection(_ilist)
    {
    }

    ~LinkedList() noexcept override
    {
    }

public:
    TLinkedList& operator=(const TLinkedList& _other)
    {
        this->CopyFrom(_other);
        return *this;
    }

    TLinkedList& operator=(TLinkedList&& _other) noexcept
    {
        this->CopyFrom(Move(_other));
        return *this;
    }

    TLinkedList& operator=(std::initializer_list<T> _ilist)
    {
        this->CopyFrom(_ilist);
        return *this;
    }

    void PushBack(const T& _data) override
    {
        TListCollection::PushBack(_data);
    }

    void PushBack(T&& _data) override
    {
        TListCollection::PushBack(Move(_data));
    }

    void PushBackAll(const TCollection& _collection) override
    {
        TListCollection::PushBackAll(_collection);
    }

    void PushFront(const T& _data) override
    {
        TListCollection::PushFront(_data);
    }

    void PushFront(T&& _data) override
    {
        TListCollection::PushFront(Move(_data));
    }

    void PushFrontAll(const TCollection& _collection) override
    {
        TListCollection::PushFrontAll(_collection);
    }

    template <typename... TArgs>
    void EmplaceBack(TArgs&&... _args)
    {
        TListCollection::EmplaceBack(Forward<TArgs>(_args)...);
    }

    template <typename... TArgs>
    void EmplaceFront(TArgs&&... _args)
    {
        TListCollection::EmplaceFront(Forward<TArgs>(_args)...);
    }

    T& Back() const override
    {
        return TListCollection::Back();
    }

    T& Front() const override
    {
        return TListCollection::Front();
    }

    void PopBack() override
    {
        TListCollection::PopBack();
    }

    void PopFront() override
    {
        TListCollection::PopFront();
    }

    bool Exist(const T& _data) const
    {
        return this->FindNode(_data) != nullptr;
    }

    bool Remove(const T& _data)
    {
        return TListCollection::Remove(_data);
    }

    bool Remove(const TLinkedListIterator& _iter)
    {
        return TListCollection::Remove(_iter);
    }

    template <typename TPredicate>
    bool RemoveIf(TPredicate&& _predicate)
    {
        TListNode* pDel = TListCollection::FindNodeIf(Move(_predicate));

        if (pDel == nullptr)
            return false;

        this->RemoveNode(pDel);
        pDel->DeleteSelf();
        --this->size_;
        return true;
    }

    TEnumerator Begin() const override
    {
        return MakeShared<TLinkedListIterator, TAllocator>(this->GetOwner(), this->pHead_);
    }

    TEnumerator End() const override
    {
        return MakeShared<TLinkedListIterator, TAllocator>(this->GetOwner(), this->pTail_);
    }

    ContainerType GetContainerType() override
    {
        return ContainerType::LinkedList;
    }

protected:
    friend class TLinkedListIterator;
    template <typename, typename, typename> friend class CHashMapIterator;
};

NS_END
