/*
 * 작성자: 윤정도
 * 생성일: 8/15/2023 5:57:04 AM
 * =====================
 *
 */

#pragma once

#include <JCore/Container/SetCollection.h>
#include <JCore/Container/TreeSetIterator.h>
#include <JCore/Container/TreeTable.h>

NS_JC_BEGIN

template <typename TKey, typename TKeyComparator = Comparator<TKey>, typename TAllocator = CDefaultAllocator, ETreeTableImplementation Implementation = ETreeTableImplementation::RedBlackTree>
class TreeSet : public SetCollection<TKey, TAllocator>
{
  public:
    using TKyComparator = TKeyComparator;
    using TTreeNode = TreeNode<TKey>;
    using TTreeTable = TreeTable<ParameterPack_t<TKey, TKeyComparator, TAllocator>, Implementation>;

    using TIterator = Iterator<TKey, TAllocator>;
    using TEnumerator = SharedPtr<TIterator>;
    using TTreeSet = TreeSet<TKey, TKeyComparator, TAllocator, Implementation>;
    using TTreeSetIterator = TreeSetIterator<TKey, TKeyComparator, TAllocator, Implementation>;

  public:
    TreeSet()
    {
    }
    TreeSet(const TTreeSet& _other)
    {
        operator=(_other);
    }

    TreeSet(TTreeSet&& _other) noexcept
    {
        operator=(Move(_other));
    }

    TreeSet(std::initializer_list<TKey> _ilist)
    {
        operator=(_ilist);
    }

    ~TreeSet() noexcept override
    {
        TTreeSet::Clear();
    }

    TTreeSet& operator=(const TTreeSet& _other)
    {
        table_.operator=(_other.table_);
        return *this;
    }

    TTreeSet& operator=(TTreeSet&& _other) noexcept
    {
        this->owner_ = Move(_other.owner_);
        table_.operator=(Move(_other.table_));
        return *this;
    }

    TTreeSet& operator=(std::initializer_list<TKey> _ilist)
    {
        table_.operator=(_ilist);
        return *this;
    }

    bool Insert(const TKey& _key) override
    {
        return table_.Insert(_key);
    }

    bool Insert(TKey&& _key) override
    {
        return table_.Insert(Move(_key));
    }

    bool Exist(const TKey& _key) const override
    {
        return table_.Exist(_key);
    }

    // 실제 삭제되는 노드가 달라질 수 있어서 이터레이터를 올바로 재설정해줘야한다.
    bool RemoveByIterator(TEnumerator& _iterator)
    {
        if(!_iterator->IsValid())
        {
            return false;
        }

        // https://stackoverflow.com/questions/610245/where-and-why-do-i-have-to-put-the-template-and-typename-_keywords
        TTreeSetIterator* pIt = _iterator.template Get<TTreeSetIterator*>();
        TKey temp = pIt->m_pIteratorNode->data_;
        table_.RemoveByNode(pIt->m_pIteratorNode);
        pIt->m_pIteratorNode = table_.pRoot_ == nullptr ? nullptr : table_.UpperBoundNode(table_.pRoot_, temp);
        return true;
    }

    bool Remove(const TKey& _key) override
    {
        return table_.Remove(_key);
    }

    void Clear() noexcept override
    {
        table_.Clear();
    }

    int MaxHeight() const
    {
        return table_.MaxHeight();
    }

    TKey* LowerBound(const TKey& _key) const
    {
        return table_.LowerBound(_key);
    }

    SharedPtr<TIterator> LowerBoundIterator(const TKey& _key) const
    {
        TTreeNode* pNode = LowerBoundNode(table_.pRoot_, _key);
        return MakeShared<TTreeSetIterator, TAllocator>(this->GetOwner(), pNode);
    }

    TKey* UpperBound(const TKey& _key) const
    {
        return table_.UpperBound(_key);
    }

    SharedPtr<TIterator> UpperBoundIterator(const TKey& _key) const
    {
        TTreeNode* pNode = table_.UpperBoundNode(table_.pRoot_, _key);
        return MakeShared<TTreeSetIterator, TAllocator>(this->GetOwner(), pNode);
    }

    // ==========================================
    // 동적할당 안하고 트리맵 순회할 수 있도록 기능 구현
    // ==========================================
    template <typename Consumer>
    void ForEach(Consumer&& consumer)
    {
        table_.ForEach(Forward<Consumer>(consumer));
    }

    void ForEachDelete()
    {
        table_.ForEachDelete();
    }

    bool TryGetFirst(OUT TKey& _key) const
    {
        return table_.TryGetFirst(_key);
    }

    bool TryGetLast(OUT TKey& _key) const
    {
        return table_.TryGetLast(_key);
    }

    TEnumerator Begin() const override
    {
        return MakeShared<TTreeSetIterator, TAllocator>(this->GetOwner(), table_.FindSmallestNode(table_.pRoot_));
    }
    TEnumerator End() const override
    {
        return MakeShared<TTreeSetIterator, TAllocator>(this->GetOwner(), table_.FindBiggestNode(table_.pRoot_));
    }
    ContainerType GetContainerType() override
    {
        return ContainerType::TreeSet;
    }

    bool IsEmpty() const override
    {
        return table_.IsEmpty();
    }
    int Size() const override
    {
        return table_.Size();
    }

    TTreeTable table_;

    friend class TTreeSetIterator;
}; // class TreeSet<TKey>

NS_JC_END
