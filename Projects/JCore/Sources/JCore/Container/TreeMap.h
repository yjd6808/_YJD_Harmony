/*
 *	작성자 : 윤정도
 *	생성일 : 2023/06/01
 *	레드블랙트리 학습결과 보고서: https://blog.naver.com/reversing_joa/223116951373
 */

#pragma once


#include <JCore/Container/MapCollection.h>
#include <JCore/Container/TreeTable.h>
#include <JCore/Container/TreeMapIterator.h>

NS_JC_BEGIN

template <typename TKey, typename TValue, typename TKeyComparator = Comparator<TKey>, typename TAllocator = DefaultAllocator, TreeTableImplementation Implementation = TreeTableImplementation::RedBlackTree>
class TreeMap : public MapCollection<TKey, TValue, TAllocator>
{
public:
	using TKeyValuePair				= Pair<TKey, TValue>;
	using TKyComparator				= TKeyComparator;
	using TTreeNode					= TreeNode<TKeyValuePair>;
	using TMapCollection			= MapCollection<TKey, TValue, TAllocator>;
	using TTreeTable				= TreeTable<ParameterPack_t<TKey, TValue, TKeyComparator, TAllocator>, Implementation>;
	
	using TIterator					= Iterator<TKeyValuePair, TAllocator>;
	using TEnumerator				= SharedPtr<TIterator>;
	using TTreeMap					= TreeMap<TKey, TValue, TKeyComparator, TAllocator, Implementation>;
	using TTreeMapIterator			= TreeMapIterator<TKey, TValue, TKeyComparator, TAllocator, Implementation>;
	using TKeyCollection			= typename TMapCollection::KeyCollection;
	using TValueCollection			= typename TMapCollection::ValueCollection;
	using TKeyCollectionIterator	= typename TMapCollection::KeyCollectionIterator;
	using TValueCollectionIterator	= typename TMapCollection::ValueCollectionIterator;
public:
	// 내부 구조체 전방 선언 (inner struct forward declaration)
	struct TreeMapKeyCollection;
	struct TreeMapKeyCollectionIterator;
	struct TreeMapValueCollection;
	struct TreeMapValueCollectionIterator;
public:
	TreeMap() {}

	TreeMap(const TTreeMap& other) : TreeMap() {
		operator=(other);
	}

	TreeMap(TTreeMap&& other) noexcept : TreeMap() {
		operator=(Move(other));
	}

	TreeMap(std::initializer_list<TKeyValuePair> ilist) : TreeMap() {
		operator=(ilist);
	}

	~TreeMap() noexcept override {
		TTreeMap::Clear();
	}

	TTreeMap& operator=(const TTreeMap& other) {
		Table.operator=(other.Table);
		return *this;
	}

	TTreeMap& operator=(TTreeMap&& other) noexcept {
		this->m_Owner = Move(other.m_Owner);
		Table.operator=(Move(other.Table));
		return *this;
	}

	TTreeMap& operator=(std::initializer_list<TKeyValuePair> ilist) {
		Table.operator=(ilist);
		return *this;
	}

	TValue& operator[](const TKey& key) {
		return Table.Get(key);
	}


	template <typename Ky, typename Vy>
	bool Insert(Ky&& key, Vy&& value) {
		return Table.Insert(Forward<Ky>(key), Forward<Vy>(value));
	}

	bool Insert(const TKeyValuePair& pair) override {
		return Table.Insert(pair);
	}

	bool Insert(TKeyValuePair&& pair) override {
		return Table.Insert(Move(pair));
	}

	bool Exist(const TKey& key) const override {
		return Table.Exist(key);
	}

	virtual TValue* Find(const TKey& key) const {
		return Table.Find(key);
	}

	TValue& Get(const TKey& key) const override {
		return Table.Get(key);
	}

	// 실제 삭제되는 노드가 달라질 수 있어서 이터레이터를 올바로 재설정해줘야한다.
	bool RemoveByIterator(TEnumerator& iterator) {
		if (!iterator->IsValid()) {
			return false;
		}

		// https://stackoverflow.com/questions/610245/where-and-why-do-i-have-to-put-the-template-and-typename-keywords
		TTreeMapIterator* pIt = iterator.template Get<TTreeMapIterator*>();
		TKey temp = pIt->m_pIteratorNode->Data.Key;
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

	TKeyValuePair* LowerBoundPair(const TKey& key) const {
		return Table.LowerBoundPair(key);
	}

	TValue* LowerBoundValue(const TKey& key) const {
		return Table.LowerBoundValue(key);
	}

	SharedPtr<TIterator> LowerBoundIterator(const TKey& key) const {
		TTreeNode* pNode = LowerBoundNode(Table.m_pRoot, key);
		return MakeShared<TTreeMapIterator, TAllocator>(this->GetOwner(), pNode);
	}

	TKeyValuePair* UpperBoundPair(const TKey& key) const {
		return Table.UpperBoundPair(key);
	}

	TValue* UpperBoundValue(const TKey& key) const {
		return Table.UpperBoundValue(key);
	}

	TKey* UpperBoundKey(const TKey& key) const {
		return Table.UpperBoundKey(key);
	}

	SharedPtr<TIterator> UpperBoundIterator(const TKey& key) const {
		TTreeNode* pNode = Table.UpperBoundNode(Table.m_pRoot, key);
		return MakeShared<TTreeMapIterator, TAllocator>(this->GetOwner(), pNode);
	}

	// ==========================================
	// 동적할당 안하고 트리맵 순회할 수 있도록 기능 구현
	// ==========================================
	template <typename Consumer>
	void ForEach(Consumer&& consumer) {
		Table.ForEach(Forward<Consumer>(consumer));
	}

	template <typename Consumer>
	void ForEachKey(Consumer&& consumer) {
		Table.ForEachKey(Forward<Consumer>(consumer));
	}

	template <typename Consumer>
	void ForEachValue(Consumer&& consumer) {
		Table.ForEachValue(Forward<Consumer>(consumer));
	}

	void ForEachValueDelete() {
		Table.ForEachValueDelete();
	}

	bool TryGetFirst(JCORE_OUT TKeyValuePair& pair) const {
		return Table.TryGetFirst(pair);
	}

	bool TryGetLast(JCORE_OUT TKeyValuePair& pair) const {
		return Table.TryGetLast(pair);
	}

	bool TryGetFirstValue(JCORE_OUT TValue& value) const {
		return Table.TryGetFirstValue(value);
	}

	bool TryGetLastValue(JCORE_OUT TValue& value) const {
		return Table.TryGetLastValue(value);
	}

	bool TryGetFirstKey(JCORE_OUT TKey& key) const {
		return Table.TryGetFirstKey(key);
	}

	TEnumerator Begin() const override { return MakeShared<TTreeMapIterator, TAllocator>(this->GetOwner(), Table.FindSmallestNode(Table.m_pRoot)); }
	TEnumerator End() const override { return MakeShared<TTreeMapIterator, TAllocator>(this->GetOwner(), Table.FindBiggestNode(Table.m_pRoot)); }
	TreeMapKeyCollection Keys() { return TreeMapKeyCollection(this); }
	TreeMapValueCollection Values() { return TreeMapValueCollection(this); }
	ContainerType GetContainerType() override { return ContainerType::TreeMap; }

	bool IsEmpty() const override { return Table.IsEmpty(); }
	int Size() const override { return Table.Size(); }

	TTreeTable Table;
public:
	struct TreeMapKeyCollection : public TKeyCollection
	{
		using TEnumerator		= SharedPtr<Iterator<TKey, TAllocator>>;
		using TCollection		= Collection<TKey, TAllocator>;

		TreeMapKeyCollection(TTreeMap* hashMap) : TKeyCollection(hashMap) {
			m_pTreeMap = hashMap;
		}

		TreeMapKeyCollection& operator=(const TreeMapKeyCollection& other) {
			this->m_pTreeMap = other.m_pTreeMap;
			this->m_pMap = other.m_pTreeMap;
			return *this;
		}

		virtual ~TreeMapKeyCollection() noexcept override = default;

		int Size() const override {
			return TKeyCollection::Size();
		}

		bool IsEmpty() const override {
			return TKeyCollection::IsEmpty();
		}

		TEnumerator Begin() const override { return MakeShared<TreeMapKeyCollectionIterator, TAllocator>(m_pTreeMap->GetOwner(), TTreeTable::FindSmallestNode(m_pTreeMap->Table.m_pRoot)); }
		TEnumerator End() const override { return MakeShared<TreeMapKeyCollectionIterator, TAllocator>(m_pTreeMap->GetOwner(), TTreeTable::FindBiggestNode(m_pTreeMap->Table.m_pRoot)); }

		ContainerType GetContainerType() override { return ContainerType::TreeMapKeyCollection; }

		TTreeMap* m_pTreeMap;
	};

	struct TreeMapKeyCollectionIterator final : public TKeyCollectionIterator
	{
		TreeMapKeyCollectionIterator(VoidOwner& owner, TTreeNode* iteratorNode)
			: TKeyCollectionIterator(owner, &m_TreeMapIterator)
			, m_TreeMapIterator(owner, iteratorNode)
		{}
		virtual ~TreeMapKeyCollectionIterator() noexcept = default;
		TTreeMapIterator m_TreeMapIterator;
	};

	struct TreeMapValueCollection final : public TValueCollection
	{
		using TEnumerator		= SharedPtr<Iterator<TValue, TAllocator>>;
		using TCollection		= Collection<TValue, TAllocator>;

		TreeMapValueCollection(TTreeMap* treeMap) : TMapCollection::ValueCollection(treeMap) {
			m_pTreeMap = treeMap;
		}

		virtual ~TreeMapValueCollection() noexcept override = default;

		TreeMapValueCollection& operator=(const TreeMapValueCollection& other) {
			this->m_pTreeMap = other.m_pTreeMap;
			this->m_pMap = other.m_pTreeMap;
			return *this;
		}


		TEnumerator Begin() const override { return MakeShared<TreeMapValueCollectionIterator, TAllocator>(m_pTreeMap->GetOwner(), TTreeTable::FindSmallestNode(m_pTreeMap->Table.m_pRoot)); }
		TEnumerator End() const override { return MakeShared<TreeMapValueCollectionIterator, TAllocator>(m_pTreeMap->GetOwner(), TTreeTable::FindBiggestNode(m_pTreeMap->Table.m_pRoot)); }
		ContainerType GetContainerType() override { return ContainerType::TreeMapValueCollection; }
		TTreeMap* m_pTreeMap;
	};

	struct TreeMapValueCollectionIterator final : public TValueCollectionIterator
	{
		TreeMapValueCollectionIterator(VoidOwner& owner, TTreeNode* iteratorNode)
			: TValueCollectionIterator(owner, &m_TreeMapIterator)
			, m_TreeMapIterator(owner, iteratorNode)
		{}
		virtual ~TreeMapValueCollectionIterator() noexcept override = default;
		TTreeMapIterator m_TreeMapIterator;
	};


	friend class TTreeMapIterator;
	friend struct TreeMapKeyCollection;
	friend struct TreeMapKeyCollectionIterator;
	friend struct TreeMapValueCollection;
	friend struct TreeMapValueCollectionIterator;
}; // class TreeMap<TKey, TValue>

NS_JC_END

