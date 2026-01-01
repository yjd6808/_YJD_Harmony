/*
	작성자 : 윤정도
*/

#pragma once

#include <jc/Container/MapCollectionIterator.h>

NS_JC_BEGIN

// 전방 선언
enum class ETreeTableImplementation;
class CVoidOwner;
template <typename> class TreeNode;
template <typename, typename> struct Pair;
template <typename, typename, typename, typename, ETreeTableImplementation> class TreeMap;

template <typename TKey, typename TValue, typename TKeyComparator, typename TAllocator, ETreeTableImplementation Implementation>
class TreeMapIterator : public MapCollectionIterator<TKey, TValue, TAllocator>
{
	using TKeyValuePair			 = Pair<TKey, TValue>;
	using TTreeNode				 = TreeNode<TKeyValuePair>;
	using TTreeMap				 = TreeMap<TKey, TValue, TKeyComparator, TAllocator, Implementation>;
	using TTreeTable			 = TreeTable<ParameterPack_t<TKey, TValue, TKeyComparator, TAllocator>, Implementation>;
	using TTreeMapIterator		 = TreeMapIterator<TKey, TValue, TKeyComparator, TAllocator, Implementation>;
	using TMapCollectionIterator = MapCollectionIterator<TKey, TValue, TAllocator>;
public:
	TreeMapIterator(CVoidOwner& _owner, TTreeNode* _pIteratorNode)
		: TMapCollectionIterator(_owner)
	{
		pIteratorNode_ = _pIteratorNode;
	}

	~TreeMapIterator() noexcept override = default;
public:
	bool HasNext() const override
	{
		if (!this->IsValid())
			return false;

		return pIteratorNode_ != nullptr;
	}

	bool HasPrevious() const override
	{
		if (!this->IsValid())
			return false;

		return pIteratorNode_ != nullptr;
	}

	TKeyValuePair& Next() override
	{
		if (pIteratorNode_ == nullptr)
			throw InvalidOperationException("데이터가 없습니다.");

		TKeyValuePair& pair = pIteratorNode_->data_;
		pIteratorNode_ = TTreeTable::FindBiggerNode(pIteratorNode_);
		return pair;
	}

	TKeyValuePair& Previous() override
	{
		if (pIteratorNode_ == nullptr)
			throw InvalidOperationException("데이터가 없습니다.");

		TKeyValuePair& pair = pIteratorNode_->data_;
		pIteratorNode_ = TTreeTable::FindSmallerNode(pIteratorNode_);
		return pair;
	}

	TKeyValuePair& Current() override
	{
		if (pIteratorNode_ == nullptr)
			throw InvalidOperationException("데이터가 없습니다.");

		return pIteratorNode_->data_;
	}

	// TODO: 올바르게 수정
	bool IsEnd() const override
	{
		return HasNext() == false;
	}

	bool IsBegin() const override
	{
		return HasPrevious() == false;
	}
protected:
	TTreeNode* pIteratorNode_;

	friend class TTreeMap;
};

NS_JC_END
