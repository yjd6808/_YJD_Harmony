/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/Container/MapCollectionIterator.h>

namespace JCore {

// ���� ����
class VoidOwner; 
template <typename> struct Hasher;
template <typename, typename> class  HashMap; 
template <typename, typename> struct Bucket;
template <typename, typename> struct BucketNode;
template <typename, typename> struct KeyValuePair;

template <typename TKey, typename TValue>
class HashMapIterator : public MapCollectionIterator<TKey, TValue>
{
	using TBucket				 = typename Bucket<TKey, TValue>;
	using TBucketNode			 = typename BucketNode<TKey, TValue>;
	using TListNode				 = typename ListNode<BucketNode<TKey, TValue>>;
	using THashMap				 = typename HashMap<TKey, TValue>;
	using TKeyValuePair			 = typename KeyValuePair<TKey, TValue>;
	using TMapCollectionIterator = typename MapCollectionIterator<TKey, TValue>;
public:
	HashMapIterator(VoidOwner& owner, TBucket* currentBucket, TListNode* currentNode) : TMapCollectionIterator(owner) {
		m_pMap = CastHashMap();

		m_pCurrentBucket = currentBucket;
		m_pCurrentNode = currentNode;
	}
	virtual ~HashMapIterator() noexcept = default;
public:
	virtual bool HasNext() const {
		if (!this->IsValid()) {
			return false;
		}

		if (m_pCurrentNode->Next == nullptr) {
			return m_pCurrentBucket->Next != nullptr;
		}

		return m_pCurrentNode->Next != nullptr;
	}

	virtual bool HasPrevious() const {
		if (!this->IsValid()) {
			return false;
		}

		if (m_pCurrentNode->Previous == m_pCurrentBucket->m_pHead) {
			return m_pCurrentBucket->Previous;
		}

		return m_pCurrentNode->Previous != m_pCurrentBucket->m_pHead;
	}

	virtual TKeyValuePair& Next() {
		// �ݺ��ڰ� �������� �����ߴµ� �����͸� �����÷��� �õ��ϴ� ���
		if (m_pCurrentBucket == m_pMap->m_pTailBucket) {
			throw InvalidOperationException("�����Ͱ� �����ϴ�.");
		}

		TBucketNode& val = m_pCurrentNode->Value;
		m_pCurrentNode = m_pCurrentNode->Next;

		if (m_pCurrentNode == m_pCurrentBucket->m_pTail) {
			m_pCurrentBucket = m_pCurrentBucket->Next;
			m_pCurrentNode = m_pCurrentBucket->m_pHead->Next;
		}

		return val.Pair;
	}

	virtual TKeyValuePair& Previous() {
		// �ݺ��ڰ� �������� �����ߴµ� �����͸� �����÷��� �õ��ϴ� ���
		if (m_pCurrentNode->Previous == m_pMap->m_pHeadBucket->m_pHead) {
			throw InvalidOperationException("�����Ͱ� �����ϴ�.");
		}

		m_pCurrentNode = m_pCurrentNode->Previous;

		TBucketNode& val = m_pCurrentNode->Value;
		if (m_pCurrentNode->Previous == m_pCurrentBucket->m_pHead) {
			m_pCurrentBucket = m_pCurrentBucket->Previous;
			m_pCurrentNode = m_pCurrentBucket->m_pTail;
		}
		return val.Pair;
	}

	virtual bool IsEnd() const {
		return HasNext() == false;
	}

	virtual bool IsBegin() const {
		return HasPrevious() == false;
	}

protected:
	THashMap* CastHashMap() const {
		this->ThrowIfIteratorIsNotValid();
		return this->Watcher.Get<THashMap>();
	}
protected:
	TListNode* m_pCurrentNode;
	TBucket* m_pCurrentBucket;
	THashMap* m_pMap;
	friend class THashMap;
};

} // namespace JCore