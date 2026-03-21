/*
	작성자 : 윤정도
*/

#pragma once


#include "jc/Container/TreeTable.h"
#include "jc/Container/SetCollectionIterator.h"

NS_JC_BEGIN

// 전방 선언
enum class ETreeTableImplementation;
class CVoidOwner;
template <typename> class TreeNode;
template <typename, typename, typename, ETreeTableImplementation> class TreeSet;

template <typename TKey, typename TKeyComparator, typename TAllocator, ETreeTableImplementation Implementation>
class TreeSetIterator : public SetCollectionIterator<TKey, TAllocator>
{
	using TTreeNode			 = TreeNode<TKey>;
	using TTreeSet			 = TreeSet<TKey, TKeyComparator, TAllocator, Implementation>;
	using TTreeTable		 = TreeTable<ParameterPack_t<TKey, TKeyComparator, TAllocator>, Implementation>;
	using TTreeSetIterator	 = TreeSetIterator<TKey, TKeyComparator, TAllocator, Implementation>;
	using TSetCollectionIterator = SetCollectionIterator<TKey, TAllocator>;
public:
	TreeSetIterator(CVoidOwner& _owner, TTreeNode* _pIteratorNode)
	: TSetCollectionIterator(_owner)
	{
		pIteratorNode_ = _pIteratorNode;
	}

	~TreeSetIterator() noexcept override = default;
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

	TKey& Next() override
	{
		if (pIteratorNode_ == nullptr)
			throw InvalidOperationException("데이터가 없습니다.");

		TKey& key = pIteratorNode_->data_;
		pIteratorNode_ = TTreeTable::FindBiggerNode(pIteratorNode_);
		return key;
	}

	TKey& Previous() override
	{
		if (pIteratorNode_ == nullptr)
			throw InvalidOperationException("데이터가 없습니다.");

		TKey& key = pIteratorNode_->data_;
		pIteratorNode_ = TTreeTable::FindSmallerNode(pIteratorNode_);
		return key;
	}

	TKey& Current() override
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

	friend class TTreeSet;
};

NS_END
