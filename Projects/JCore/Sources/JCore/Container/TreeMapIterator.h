/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/MapCollectionIterator.h>

NS_JC_BEGIN

// 전방 선언
enum class TreeTableImplementation;
class VoidOwner; 
template <typename> struct TreeNode;
template <typename, typename, typename, typename, TreeTableImplementation> class TreeMap;
template <typename, typename> struct Pair;

template <typename TKey, typename TValue, typename TKeyComparator, typename TAllocator, TreeTableImplementation Implementation>
class TreeMapIterator : public MapCollectionIterator<TKey, TValue, TAllocator>
{
	using TKeyValuePair			 = Pair<TKey, TValue>;
	using TTreeNode				 = TreeNode<TKeyValuePair>;
	using TTreeMap				 = TreeMap<TKey, TValue, TKeyComparator, TAllocator, Implementation>;
	using TTreeTable			 = TreeTable<ParameterPack_t<TKey, TValue, TKeyComparator, TAllocator>, Implementation>;
	using TTreeMapIterator		 = TreeMapIterator<TKey, TValue, TKeyComparator, TAllocator, Implementation>;
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

		TKeyValuePair& pair = m_pIteratorNode->Data;
		m_pIteratorNode = TTreeTable::FindBiggerNode(m_pIteratorNode);
		return pair;
	}

	TKeyValuePair& Previous() override {
		if (m_pIteratorNode == nullptr) {
			throw InvalidOperationException("데이터가 없습니다.");
		}

		TKeyValuePair& pair = m_pIteratorNode->Data;
		m_pIteratorNode = TTreeTable::FindSmallerNode(m_pIteratorNode);
		return pair;
	}

	TKeyValuePair& Current() override {
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

	friend class TTreeMap;
};

NS_JC_END