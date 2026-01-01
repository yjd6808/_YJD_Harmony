/*
 *	작성자 : 윤정도
 *	생성일 : 2023/06/01
 *	레드블랙트리 학습결과 보고서: https://blog.naver.com/reversing_joa/223116951373
 */

#pragma once


#include <jc/Container/MapCollection.h>
#include <jc/Container/TreeTable.h>
#include <jc/Container/TreeMapIterator.h>

NS_JC_BEGIN

template <typename TKey, typename TValue, typename TKeyComparator = Comparator<TKey>, typename TAllocator = CDefaultAllocator, ETreeTableImplementation Implementation = ETreeTableImplementation::RedBlackTree>
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

	TreeMap(const TTreeMap& _other) : TreeMap()
	{
		operator=(_other);
	}

	TreeMap(TTreeMap&& _other) noexcept : TreeMap()
	{
		operator=(Move(_other));
	}

	TreeMap(std::initializer_list<TKeyValuePair> _ilist) : TreeMap()
	{
		operator=(_ilist);
	}

	~TreeMap() noexcept override
	{
		TTreeMap::Clear();
	}

	TTreeMap& operator=(const TTreeMap& _other)
	{
		treeTable_.operator=(_other.treeTable_);
		return *this;
	}

	TTreeMap& operator=(TTreeMap&& _other) noexcept
	{
		this->owner_ = Move(_other.owner_);
		treeTable_.operator=(Move(_other.treeTable_));
		return *this;
	}

	TTreeMap& operator=(std::initializer_list<TKeyValuePair> _ilist)
	{
		treeTable_.operator=(_ilist);
		return *this;
	}

	TValue& operator[](const TKey& _key)
	{
		return treeTable_.Get(_key);
	}


	template <typename Ky, typename Vy>
	bool Insert(Ky&& _key, Vy&& _value)
	{
		return treeTable_.Insert(Forward<Ky>(_key), Forward<Vy>(_value));
	}

	bool Insert(const TKeyValuePair& _pair) override
	{
		return treeTable_.Insert(_pair);
	}

	bool Insert(TKeyValuePair&& _pair) override
	{
		return treeTable_.Insert(Move(_pair));
	}

	bool Exist(const TKey& _key) const override
	{
		return treeTable_.Exist(_key);
	}

	virtual TValue* Find(const TKey& _key) const
	{
		return treeTable_.Find(_key);
	}

	TValue& Get(const TKey& _key) const override
	{
		return treeTable_.Get(_key);
	}

	// 실제 삭제되는 노드가 달라질 수 있어서 이터레이터를 올바로 재설정해줘야한다.
	bool RemoveByIterator(TEnumerator& _iterator)
	{
		if (!_iterator->IsValid())
		{
			return false;
		}

		// https://stackoverflow.com/questions/610245/where-and-why-do-i-have-to-put-the-template-and-typename-keywords
		TTreeMapIterator* pIterator = _iterator.template Get<TTreeMapIterator*>();
		TKey temp = pIterator->pIteratorNode_->data_.key_;
		treeTable_.RemoveByNode(pIterator->pIteratorNode_);
		pIterator->pIteratorNode_= treeTable_.pRoot_ == nullptr ? nullptr : treeTable_.UpperBoundNode(treeTable_.pRoot_, temp);
		return true;
	}


	bool Remove(const TKey& _key) override
	{
		return treeTable_.Remove(_key);
	}


	void Clear() noexcept override
	{
		treeTable_.Clear();
	}

	int MaxHeight() const
	{
		return treeTable_.MaxHeight();
	}

	TKeyValuePair* LowerBoundPair(const TKey& _key) const
	{
		return treeTable_.LowerBoundPair(_key);
	}

	TValue* LowerBoundValue(const TKey& _key) const
	{
		return treeTable_.LowerBoundValue(_key);
	}

	SharedPtr<TIterator> LowerBoundIterator(const TKey& _key) const
	{
		TTreeNode* pNode = LowerBoundNode(treeTable_.pRoot_, _key);
		return MakeShared<TTreeMapIterator, TAllocator>(this->GetOwner(), pNode);
	}

	TKeyValuePair* UpperBoundPair(const TKey& _key) const
	{
		return treeTable_.UpperBoundPair(_key);
	}

	TValue* UpperBoundValue(const TKey& _key) const
	{
		return treeTable_.UpperBoundValue(_key);
	}

	TKey* UpperBoundKey(const TKey& _key) const
	{
		return treeTable_.UpperBoundKey(_key);
	}

	SharedPtr<TIterator> UpperBoundIterator(const TKey& _key) const
	{
		TTreeNode* pNode = treeTable_.UpperBoundNode(treeTable_.pRoot_, _key);
		return MakeShared<TTreeMapIterator, TAllocator>(this->GetOwner(), pNode);
	}

	// ==========================================
	// 동적할당 안하고 트리맵 순회할 수 있도록 기능 구현
	// ==========================================
	template <typename Consumer>
	void ForEach(Consumer&& _consumer)
	{
		treeTable_.ForEach(Forward<Consumer>(_consumer));
	}

	template <typename Consumer>
	void ForEachKey(Consumer&& _consumer)
	{
		treeTable_.ForEachKey(Forward<Consumer>(_consumer));
	}

	template <typename Consumer>
	void ForEachValue(Consumer&& _consumer)
	{
		treeTable_.ForEachValue(Forward<Consumer>(_consumer));
	}

	void ForEachValueDelete()
	{
		treeTable_.ForEachValueDelete();
	}

	bool TryGetFirst(OUT TKeyValuePair& _pair) const
	{
		return treeTable_.TryGetFirst(_pair);
	}

	bool TryGetLast(OUT TKeyValuePair& _pair) const
	{
		return treeTable_.TryGetLast(_pair);
	}

	bool TryGetFirstValue(OUT TValue& _value) const
	{
		return treeTable_.TryGetFirstValue(_value);
	}

	bool TryGetLastValue(OUT TValue& _value) const
	{
		return treeTable_.TryGetLastValue(_value);
	}

	bool TryGetFirstKey(OUT TKey& _key) const
	{
		return treeTable_.TryGetFirstKey(_key);
	}

	TEnumerator Begin() const override { return MakeShared<TTreeMapIterator, TAllocator>(this->GetOwner(), treeTable_.FindSmallestNode(treeTable_.pRoot_)); }
	TEnumerator End() const override { return MakeShared<TTreeMapIterator, TAllocator>(this->GetOwner(), treeTable_.FindBiggestNode(treeTable_.pRoot_)); }
	TreeMapKeyCollection Keys() { return TreeMapKeyCollection(this); }
	TreeMapValueCollection Values() { return TreeMapValueCollection(this); }
	ContainerType GetContainerType() override { return ContainerType::TreeMap; }

	bool IsEmpty() const override { return treeTable_.IsEmpty(); }
	int Size() const override { return treeTable_.Size(); }

	TTreeTable treeTable_;
public:
	struct TreeMapKeyCollection : public TKeyCollection
	{
		using TEnumerator		= SharedPtr<Iterator<TKey, TAllocator>>;
		using TCollection		= Collection<TKey, TAllocator>;

		TreeMapKeyCollection(TTreeMap* _pTreeMap)
			: TKeyCollection(_pTreeMap)
		{
			pTreeMap_ = _pTreeMap;
		}

		TreeMapKeyCollection& operator=(const TreeMapKeyCollection& _other)
		{
			this->pTreeMap_ = _other.pTreeMap_;
			this->m_pMap = _other.pTreeMap_;
			return *this;
		}

		virtual ~TreeMapKeyCollection() noexcept override = default;

		int Size() const override
		{
			return TKeyCollection::Size();
		}

		bool IsEmpty() const override
		{
			return TKeyCollection::IsEmpty();
		}

		TEnumerator Begin() const override { return MakeShared<TreeMapKeyCollectionIterator, TAllocator>(pTreeMap_->GetOwner(), TTreeTable::FindSmallestNode(pTreeMap_->treeTable_.pRoot_)); }
		TEnumerator End() const override { return MakeShared<TreeMapKeyCollectionIterator, TAllocator>(pTreeMap_->GetOwner(), TTreeTable::FindBiggestNode(pTreeMap_->treeTable_.pRoot_)); }

		ContainerType GetContainerType() override { return ContainerType::TreeMapKeyCollection; }

		TTreeMap* pTreeMap_;
	};

	struct TreeMapKeyCollectionIterator final : public TKeyCollectionIterator
	{
		TreeMapKeyCollectionIterator(CVoidOwner& _owner, TTreeNode* _pIteratorNode)
			: TKeyCollectionIterator(_owner, &treeMapIterator_)
			, treeMapIterator_(_owner, _pIteratorNode)
		{}
		virtual ~TreeMapKeyCollectionIterator() noexcept override = default;
		TTreeMapIterator treeMapIterator_;
	};

	struct TreeMapValueCollection final : public TValueCollection
	{
		using TEnumerator		= SharedPtr<Iterator<TValue, TAllocator>>;
		using TCollection		= Collection<TValue, TAllocator>;

		TreeMapValueCollection(TTreeMap* _pTreeMap)
			: TMapCollection::ValueCollection(_pTreeMap)
		{
			pTreeMap_ = _pTreeMap;
		}

		virtual ~TreeMapValueCollection() noexcept override = default;

		TreeMapValueCollection& operator=(const TreeMapValueCollection& _other)
		{
			this->pTreeMap_ = _other.pTreeMap_;
			this->m_pMap = _other.pTreeMap_;
			return *this;
		}


		TEnumerator Begin() const override { return MakeShared<TreeMapValueCollectionIterator, TAllocator>(pTreeMap_->GetOwner(), TTreeTable::FindSmallestNode(pTreeMap_->treeTable_.pRoot_)); }
		TEnumerator End() const override { return MakeShared<TreeMapValueCollectionIterator, TAllocator>(pTreeMap_->GetOwner(), TTreeTable::FindBiggestNode(pTreeMap_->treeTable_.pRoot_)); }
		ContainerType GetContainerType() override { return ContainerType::TreeMapValueCollection; }
		TTreeMap* pTreeMap_;
	};

	struct TreeMapValueCollectionIterator final : public TValueCollectionIterator
	{
		TreeMapValueCollectionIterator(CVoidOwner& _owner, TTreeNode* _pIteratorNode)
			: TValueCollectionIterator(_owner, &treeMapIterator_)
			, treeMapIterator_(_owner, _pIteratorNode)
		{}
		virtual ~TreeMapValueCollectionIterator() noexcept override = default;
		TTreeMapIterator treeMapIterator_;
	};


	friend class TTreeMapIterator;
	friend struct TreeMapKeyCollection;
	friend struct TreeMapKeyCollectionIterator;
	friend struct TreeMapValueCollection;
	friend struct TreeMapValueCollectionIterator;
}; // class CTreeMap<TKey, TValue>

NS_JC_END

