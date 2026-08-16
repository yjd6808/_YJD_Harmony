/*
 *	작성자 : 윤정도
 *	생성일 : 2023/06/01
 *	레드블랙트리 학습결과 보고서: https://blog.naver.com/reversing_joa/223116951373
 */

#pragma once


#include "jc/Container/TreeTable.h"
#include "jc/Container/TreeMapIterator.h"

NS_JC_BEGIN

template <typename TKey, typename TValue, typename TKeyComparator = Comparator<TKey>, typename TAllocator = CDefaultAllocator, ETreeTableImplementation Implementation = ETreeTableImplementation::RedBlackTree>
class TreeMap
{
public:
	using TKeyValuePair				= Pair<TKey, TValue>;
	using TKyComparator				= TKeyComparator;
	using TTreeNode					= TreeNode<TKeyValuePair>;
	using TTreeTable				= TreeTable<ParameterPack_t<TKey, TValue, TKeyComparator, TAllocator>, Implementation>;
	using TTreeMap					= TreeMap<TKey, TValue, TKeyComparator, TAllocator, Implementation>;
	using TTreeMapIterator			= TreeMapIterator<TKey, TValue, TKeyComparator, TAllocator, Implementation>;
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

	~TreeMap() noexcept
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

	bool Insert(const TKeyValuePair& _pair)
	{
		return treeTable_.Insert(_pair);
	}

	bool Insert(TKeyValuePair&& _pair)
	{
		return treeTable_.Insert(Move(_pair));
	}

	bool Exist(const TKey& _key) const
	{
		return treeTable_.Exist(_key);
	}

	TValue* Find(const TKey& _key) const
	{
		return treeTable_.Find(_key);
	}

	TValue& Get(const TKey& _key) const
	{
		return treeTable_.Get(_key);
	}

	// 실제 삭제되는 노드가 달라질 수 있어서 이터레이터를 올바로 재설정해줘야한다.
	bool RemoveByIterator(TTreeMapIterator& _iterator)
	{
		if (_iterator.pIteratorNode_ == nullptr)
		{
			return false;
		}

		TKey temp = _iterator.pIteratorNode_->data_.key_;
		treeTable_.RemoveByNode(_iterator.pIteratorNode_);
		_iterator.pIteratorNode_ = treeTable_.pRoot_ == nullptr ? nullptr : treeTable_.UpperBoundNode(treeTable_.pRoot_, temp);
		return true;
	}


	bool Remove(const TKey& _key)
	{
		return treeTable_.Remove(_key);
	}


	void Clear() noexcept
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

	TTreeMapIterator LowerBoundIterator(const TKey& _key) const
	{
		TTreeNode* pNode = TTreeTable::LowerBoundNode(treeTable_.pRoot_, _key);
		return TTreeMapIterator(pNode);
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

	TTreeMapIterator UpperBoundIterator(const TKey& _key) const
	{
		TTreeNode* pNode = treeTable_.UpperBoundNode(treeTable_.pRoot_, _key);
		return TTreeMapIterator(pNode);
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

	bool TryPop(const TKey& _key, TValue* _pOut)
	{
		return treeTable_.TryPop(_key, _pOut);
	}

	TTreeMapIterator Begin() const { return TTreeMapIterator(treeTable_.FindSmallestNode(treeTable_.pRoot_)); }
	TTreeMapIterator End() const { return TTreeMapIterator(treeTable_.FindBiggestNode(treeTable_.pRoot_)); }

	auto Keys() const
	{
		return TreeMapKeyCollection(const_cast<TTreeMap*>(this));
	}

	auto Values() const
	{
		return TreeMapValueCollection(const_cast<TTreeMap*>(this));
	}

	bool IsEmpty() const { return treeTable_.IsEmpty(); }
	int Size() const { return treeTable_.Size(); }

	TTreeTable treeTable_;
public:
	struct TreeMapKeyCollection
	{
		using TKeyValuePair		= Pair<TKey, TValue>;
		using TTreeNode			= TreeNode<TKeyValuePair>;

		struct Iterator
		{
			using TTreeMapIterator = TreeMapIterator<TKey, TValue, TKeyComparator, TAllocator, Implementation>;

			Iterator(TTreeNode* _pIteratorNode)
				: treeMapIterator_(_pIteratorNode)
			{
			}

			bool HasNext() const
			{
				return treeMapIterator_.HasNext();
			}

			bool HasPrevious() const
			{
				return treeMapIterator_.HasPrevious();
			}

			TKey& Next()
			{
				return treeMapIterator_.Next().key_;
			}

			TKey& Previous()
			{
				return treeMapIterator_.Previous().key_;
			}

			TKey& Current()
			{
				return treeMapIterator_.Current().key_;
			}

			bool IsEnd() const
			{
				return treeMapIterator_.IsEnd();
			}

			bool IsBegin() const
			{
				return treeMapIterator_.IsBegin();
			}

		private:
			TTreeMapIterator treeMapIterator_;
		};

		TreeMapKeyCollection(TTreeMap* _pTreeMap)
			: pTreeMap_(_pTreeMap)
		{
		}

		TreeMapKeyCollection& operator=(const TreeMapKeyCollection& _other)
		{
			pTreeMap_ = _other.pTreeMap_;
			return *this;
		}

		int Size() const
		{
			return pTreeMap_->Size();
		}

		bool IsEmpty() const
		{
			return pTreeMap_->IsEmpty();
		}

		auto Begin() const
		{
			return Iterator(TTreeTable::FindSmallestNode(pTreeMap_->treeTable_.pRoot_));
		}

		auto End() const
		{
			return Iterator(TTreeTable::FindBiggestNode(pTreeMap_->treeTable_.pRoot_));
		}

		bool Exist(const TKey& _value) const
		{
			auto it = Begin();
			while (it.HasNext())
			{
				if (it.Next() == _value)
				{
					return true;
				}
			}
			return false;
		}

		TKey* First()
		{
			auto it = Begin();
			if (!it.HasNext())
			{
				return nullptr;
			}

			return AddressOf(it.Next());
		}

		TKey* Last()
		{
			auto it = End();
			if (!it.HasPrevious())
			{
				return nullptr;
			}

			return AddressOf(it.Previous());
		}

		TTreeMap* pTreeMap_;
	};

	struct TreeMapValueCollection
	{
		using TKeyValuePair		= Pair<TKey, TValue>;
		using TTreeNode			= TreeNode<TKeyValuePair>;

		struct Iterator
		{
			using TTreeMapIterator = TreeMapIterator<TKey, TValue, TKeyComparator, TAllocator, Implementation>;

			Iterator(TTreeNode* _pIteratorNode)
				: treeMapIterator_(_pIteratorNode)
			{
			}

			bool HasNext() const
			{
				return treeMapIterator_.HasNext();
			}

			bool HasPrevious() const
			{
				return treeMapIterator_.HasPrevious();
			}

			TValue& Next()
			{
				return treeMapIterator_.Next().value_;
			}

			TValue& Previous()
			{
				return treeMapIterator_.Previous().value_;
			}

			TValue& Current()
			{
				return treeMapIterator_.Current().value_;
			}

			bool IsEnd() const
			{
				return treeMapIterator_.IsEnd();
			}

			bool IsBegin() const
			{
				return treeMapIterator_.IsBegin();
			}

		private:
			TTreeMapIterator treeMapIterator_;
		};

		TreeMapValueCollection(TTreeMap* _pTreeMap)
			: pTreeMap_(_pTreeMap)
		{
		}

		TreeMapValueCollection& operator=(const TreeMapValueCollection& _other)
		{
			pTreeMap_ = _other.pTreeMap_;
			return *this;
		}

		int Size() const
		{
			return pTreeMap_->Size();
		}

		bool IsEmpty() const
		{
			return pTreeMap_->IsEmpty();
		}

		auto Begin() const
		{
			return Iterator(TTreeTable::FindSmallestNode(pTreeMap_->treeTable_.pRoot_));
		}

		auto End() const
		{
			return Iterator(TTreeTable::FindBiggestNode(pTreeMap_->treeTable_.pRoot_));
		}

		bool Exist(const TValue& _value) const
		{
			auto it = Begin();
			while (it.HasNext())
			{
				if (it.Next() == _value)
				{
					return true;
				}
			}
			return false;
		}

		TValue* First()
		{
			auto it = Begin();
			if (!it.HasNext())
			{
				return nullptr;
			}

			return AddressOf(it.Next());
		}

		TValue* Last()
		{
			auto it = End();
			if (!it.HasPrevious())
			{
				return nullptr;
			}

			return AddressOf(it.Previous());
		}

		TTreeMap* pTreeMap_;
	};

	friend class TTreeMapIterator;
}; // class CTreeMap<TKey, TValue>

NS_END