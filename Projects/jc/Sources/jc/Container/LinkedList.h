/*
	작성자 : 윤정도
*/

#pragma once

#include "jc/Allocator/DefaultAllocator.h"
#include "jc/Container/ListCollection.h"

NS_JC_BEGIN

template <typename T, typename TAllocator = CDefaultAllocator>
class LinkedList : public ListCollection<T, TAllocator>
{
    using TListNode = ListNode<T, TAllocator>;
    using TListCollection = ListCollection<T, TAllocator>;
    using TLinkedList = LinkedList<T, TAllocator>;

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

    ~LinkedList() noexcept
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

    bool Exist(const T& _data) const
    {
        return this->FindNode(_data) != nullptr;
    }

    bool Remove(const T& _data)
    {
        return TListCollection::Remove(_data);
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

protected:
    template <typename, typename, typename> friend class CHashMapIterator;
};

NS_END
