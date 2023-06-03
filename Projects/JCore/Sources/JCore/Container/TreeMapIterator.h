/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/MapCollectionIterator.h>

NS_JC_BEGIN

// 전방 선언
class VoidOwner; 
template <typename> struct Hasher;
template <typename, typename, typename, typename> class  TreeMap; 
template <typename, typename> struct TreeNode;
template <typename, typename> struct Pair;

template <typename TKey, typename TValue, typename TKeyComparator, typename TAllocator>
class TreeMapIterator : public MapCollectionIterator<TKey, TValue, TAllocator>
{
	using TTreeNode				 = TreeNode<TKey, TValue>;
	using TTreeMap				 = TreeMap<TKey, TValue, TKeyComparator, TAllocator>;
	using TKeyValuePair			 = Pair<TKey, TValue>;
	using TMapCollectionIterator = MapCollectionIterator<TKey, TValue, TAllocator>;
public:
	TreeMapIterator(VoidOwner& owner, TTreeNode* node) : TMapCollectionIterator(owner) { m_pIteratorNode = node; }
	~TreeMapIterator() noexcept override = default;
public:
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

	TKeyValuePair& Next() override {
		if (m_pIteratorNode == nullptr) {
			throw InvalidOperationException("데이터가 없습니다.");
		}

		TKeyValuePair& pair = m_pIteratorNode->Pair;
		m_pIteratorNode = TTreeMap::FindBiggerNode(m_pIteratorNode);
		return pair;
	}

	TKeyValuePair& Previous() override {
		if (m_pIteratorNode == nullptr) {
			throw InvalidOperationException("데이터가 없습니다.");
		}

		TKeyValuePair& pair = m_pIteratorNode->Pair;
		m_pIteratorNode = TTreeMap::FindSmallerNode(m_pIteratorNode);
		return pair;
	}

	TKeyValuePair& Current() override {
		if (m_pIteratorNode == nullptr) {
			throw InvalidOperationException("데이터가 없습니다.");
		}

		return m_pIteratorNode->Pair;
	}

	bool IsEnd() const override {
		return HasNext() == false;
	}

	bool IsBegin() const override {
		return HasPrevious() == false;
	}

protected:
	TTreeNode* m_pIteratorNode;
	friend class TTreeMap;
};

NS_JC_END