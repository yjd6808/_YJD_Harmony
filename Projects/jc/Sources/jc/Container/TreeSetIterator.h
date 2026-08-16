/*
	작성자 : 윤정도
*/

#pragma once

#include "jc/Container/TreeTable.h"

NS_JC_BEGIN

template <typename TKey, typename TKeyComparator, typename TAllocator, ETreeTableImplementation Implementation>
class TreeSetIterator
{
	using TTreeNode			 = TreeNode<TKey>;
	using TTreeSet			 = TreeSet<TKey, TKeyComparator, TAllocator, Implementation>;
	using TTreeTable		 = TreeTable<ParameterPack_t<TKey, TKeyComparator, TAllocator>, Implementation>;

public:
	TreeSetIterator(TTreeNode* _pIteratorNode)
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

	TKey& Next()
	{
		if (pIteratorNode_ == nullptr)
			throw InvalidOperationException("데이터가 없습니다.");

		TKey& key = pIteratorNode_->data_;
		pIteratorNode_ = TTreeTable::FindBiggerNode(pIteratorNode_);
		return key;
	}

	TKey& Previous()
	{
		if (pIteratorNode_ == nullptr)
			throw InvalidOperationException("데이터가 없습니다.");

		TKey& key = pIteratorNode_->data_;
		pIteratorNode_ = TTreeTable::FindSmallerNode(pIteratorNode_);
		return key;
	}

	TKey& Current()
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

	friend class TTreeSet;
};

NS_END