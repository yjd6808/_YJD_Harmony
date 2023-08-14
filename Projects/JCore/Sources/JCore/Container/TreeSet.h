/*
 * 작성자: 윤정도
 * 생성일: 8/15/2023 5:57:04 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/SetCollection.h>
#include <JCore/Container/TreeTable.h>
#include <JCore/Container/TreeSetIterator.h>

NS_JC_BEGIN

template <typename TKey, typename TKeyComparator = Comparator<TKey>, typename TAllocator = DefaultAllocator, TreeTableImplementation Implementation = TreeTableImplementation::RedBlackTree>
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
	TreeSet() {}
	TreeSet(const TTreeSet& other) {
		operator=(other);
	}

	TreeSet(TTreeSet&& other) noexcept {
		operator=(Move(other));
	}

	TreeSet(std::initializer_list<TKey> ilist) {
		operator=(ilist);
	}

	~TreeSet() noexcept override {
		TTreeSet::Clear();
	}

	TTreeSet& operator=(const TTreeSet& other) {
		Table.operator=(other.Table);
		return *this;
	}

	TTreeSet& operator=(TTreeSet&& other) noexcept {
		this->m_Owner = Move(other.m_Owner);
		Table.operator=(Move(other.Table));
		return *this;
	}

	TTreeSet& operator=(std::initializer_list<TKey> ilist) {
		Table.operator=(ilist);
		return *this;
	}

	bool Insert(const TKey& key) override {
		return Table.Insert(key);
	}

	bool Insert(TKey&& key) override {
		return Table.Insert(Move(key));
	}

	bool Exist(const TKey& key) const override {
		return Table.Exist(key);
	}

	// 실제 삭제되는 노드가 달라질 수 있어서 이터레이터를 올바로 재설정해줘야한다.
	bool RemoveByIterator(TEnumerator& iterator) {
		if (!iterator->IsValid()) {
			return false;
		}

		// https://stackoverflow.com/questions/610245/where-and-why-do-i-have-to-put-the-template-and-typename-keywords
		TTreeSetIterator* pIt = iterator.template Get<TTreeSetIterator*>();
		TKey temp = pIt->m_pIteratorNode->Data;
		Table.RemoveByNode(pIt->m_pIteratorNode);
		pIt->m_pIteratorNode = Table.m_pRoot == nullptr ? nullptr : Table.UpperBoundNode(Table.m_pRoot, temp);
		return true;
	}


	bool Remove(const TKey& key) override {
		return Table.Remove(key);
	}

	void Clear() noexcept override {
		Table.Clear();
	}

	int MaxHeight() const {
		return Table.MaxHeight();
	}

	TKey* LowerBound(const TKey& key) const {
		return Table.LowerBound(key);
	}

	SharedPtr<TIterator> LowerBoundIterator(const TKey& key) const {
		TTreeNode* pNode = LowerBoundNode(Table.m_pRoot, key);
		return MakeShared<TTreeSetIterator, TAllocator>(this->GetOwner(), pNode);
	}

	TKey* UpperBound(const TKey& key) const {
		return Table.UpperBound(key);
	}

	SharedPtr<TIterator> UpperBoundIterator(const TKey& key) const {
		TTreeNode* pNode = Table.UpperBoundNode(Table.m_pRoot, key);
		return MakeShared<TTreeSetIterator, TAllocator>(this->GetOwner(), pNode);
	}

	// ==========================================
	// 동적할당 안하고 트리맵 순회할 수 있도록 기능 구현
	// ==========================================
	template <typename Consumer>
	void ForEach(Consumer&& consumer) {
		Table.ForEach(Forward<Consumer>(consumer));
	}

	void ForEachDelete() {
		Table.ForEachDelete();
	}

	bool TryGetFirst(JCORE_OUT TKey& key) const {
		return Table.TryGetFirst(key);
	}

	bool TryGetLast(JCORE_OUT TKey& key) const {
		return Table.TryGetLast(key);
	}

	TEnumerator Begin() const override { return MakeShared<TTreeSetIterator, TAllocator>(this->GetOwner(), Table.FindSmallestNode(Table.m_pRoot)); }
	TEnumerator End() const override { return MakeShared<TTreeSetIterator, TAllocator>(this->GetOwner(), Table.FindBiggestNode(Table.m_pRoot)); }
	ContainerType GetContainerType() override { return ContainerType::TreeSet; }

	bool IsEmpty() const override { return Table.IsEmpty(); }
	int Size() const override { return Table.Size(); }

	TTreeTable Table;

	friend class TTreeSetIterator;
}; // class TreeSet<TKey>

NS_JC_END
