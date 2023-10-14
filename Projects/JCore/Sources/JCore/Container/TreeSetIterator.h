/*
 * 작성자: 윤정도
 * 생성일: 8/15/2023 5:57:14 AM
 * =====================
 *
 */


#pragma once


#include <JCore/Container/SetCollectionIterator.h>

NS_JC_BEGIN

// 전방 선언
enum class TreeTableImplementation;
class VoidOwner; 
template <typename> struct TreeNode;
template <typename, typename, typename, TreeTableImplementation> class TreeSet;

template <typename TKey, typename TKeyComparator, typename TAllocator, TreeTableImplementation Implementation>
class TreeSetIterator : public SetCollectionIterator<TKey, TAllocator>
{
	using TTreeNode				 = TreeNode<TKey>;
	using TTreeSet				 = TreeSet<TKey, TKeyComparator, TAllocator, Implementation>;
	using TTreeTable			 = TreeTable<ParameterPack_t<TKey, TKeyComparator, TAllocator>, Implementation>;
	using TTreeSetIterator		 = TreeSetIterator<TKey, TKeyComparator, TAllocator, Implementation>;
	using TSetCollectionIterator = SetCollectionIterator<TKey, TAllocator>;
public:
	TreeSetIterator(VoidOwner& owner, TTreeNode* node) : TSetCollectionIterator(owner) { m_pIteratorNode = node; }
	~TreeSetIterator() noexcept override = default;

	bool HasNext() const override {
		if (!this->IsValid())
			return false;

		return m_pIteratorNode;
	}

	bool HasPrevious() const override {
		if (!this->IsValid()) 
			return false;

		return m_pIteratorNode;
	}

	TKey& Next() override {
		if (m_pIteratorNode == nullptr) {
			throw InvalidOperationException("데이터가 없습니다.");
		}

		TKey& key = m_pIteratorNode->Data;
		m_pIteratorNode = TTreeTable::FindBiggerNode(m_pIteratorNode);
		return key;
	}

	TKey& Previous() override {
		if (m_pIteratorNode == nullptr) {
			throw InvalidOperationException("데이터가 없습니다.");
		}

		TKey& key = m_pIteratorNode->Data;
		m_pIteratorNode = TTreeTable::FindSmallerNode(m_pIteratorNode);
		return key;
	}

	TKey& Current() override {
		if (m_pIteratorNode == nullptr) {
			throw InvalidOperationException("데이터가 없습니다.");
		}

		return m_pIteratorNode->Data;
	}

	// TODO: 올바르게 수정
	bool IsEnd() const override {
		return HasNext() == false;
	}

	bool IsBegin() const override {
		return HasPrevious() == false;
	}
protected:
	TTreeNode* m_pIteratorNode;

	friend class TTreeSet;
};

NS_JC_END