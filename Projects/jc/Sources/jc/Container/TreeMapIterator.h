/*
	작성자 : 윤정도
*/

#pragma once

#include "jc/Container/TreeTable.h"

NS_JC_BEGIN

template <typename TKey, typename TValue, typename TKeyComparator, typename TAllocator, ETreeTableImplementation Implementation>
class TreeMapIterator
{
	using TKeyValuePair			 = Pair<TKey, TValue>;
	using TTreeNode				 = TreeNode<TKeyValuePair>;
	using TTreeMap				 = TreeMap<TKey, TValue, TKeyComparator, TAllocator, Implementation>;
	using TTreeTable			 = TreeTable<ParameterPack_t<TKey, TValue, TKeyComparator, TAllocator>, Implementation>;

public:
	TreeMapIterator(TTreeNode* _pIteratorNode)
		: pIteratorNode_(_pIteratorNode)
	{
	}

	bool HasNext() const
	{
		return pIteratorNode_ != nullptr;
	}

	bool HasPrevious() const
	{
		return pIteratorNode_ != nullptr;
	}

	TKeyValuePair& Next()
	{
		if (pIteratorNode_ == nullptr)
			throw InvalidOperationException("데이터가 없습니다.");

		TKeyValuePair& pair = pIteratorNode_->data_;
		pIteratorNode_ = TTreeTable::FindBiggerNode(pIteratorNode_);
		return pair;
	}

	TKeyValuePair& Previous()
	{
		if (pIteratorNode_ == nullptr)
			throw InvalidOperationException("데이터가 없습니다.");

		TKeyValuePair& pair = pIteratorNode_->data_;
		pIteratorNode_ = TTreeTable::FindSmallerNode(pIteratorNode_);
		return pair;
	}

	TKeyValuePair& Current()
	{
		if (pIteratorNode_ == nullptr)
			throw InvalidOperationException("데이터가 없습니다.");

		return pIteratorNode_->data_;
	}

	// TODO: 올바르게 수정
	bool IsEnd() const
	{
		return HasNext() == false;
	}

	bool IsBegin() const
	{
		return HasPrevious() == false;
	}

protected:
	TTreeNode* pIteratorNode_;

	friend class TTreeMap;
};

NS_END