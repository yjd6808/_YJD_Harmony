/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/Hasher.h>
#include <JCore/Memory.h>

#include <JCore/Container/MapCollection.h>
#include <JCore/Container/LinkedList.h>
#include <JCore/Container/HashMapIterator.h>

namespace JCore {

template <typename TKey, typename TValue>
struct BucketNode
{
	using TKeyValuePair = typename KeyValuePair<TKey, TValue>;

	TKeyValuePair Pair;
	Int32U Hash;				// ó���� �ѹ� ����س����� ���� ��?
};

template <typename TKey, typename TValue>
struct Bucket
{
	LinkedList<BucketNode<TKey, TValue>> Nodes;
	Bucket* Next = nullptr;
	Bucket* Previous = nullptr;

	bool Find(const TKey& key) {
		auto it = Nodes.Begin();
		while (it->HasNext()) {
			if (it->Next().Pair.Key == key) {
				return true;
			}
		}
		return false;
	}

	TValue& Get(const TKey& key) {
		auto it = Nodes.Begin();
		while (it->HasNext()) {
			auto node = it->Next();
			if (node.Pair.Key == key) {
				return node.Pair.Value;
			}
		}

		TValue v;
		return v;
	}

	bool Remove(const TKey& key) {
		auto it = Nodes.Begin();
		while (it->HasNext()) {
			auto node = it->Next();
			if (node.Pair.Key == key) {
				return true;//Nodes.Remove(it.GetObj());
			}
		}
		return false;
	}

	int Size() const {
		return Nodes.Size();
	}

	bool IsEmpty() const {
		return Nodes.Size() == 0;
	}
};

template <typename TKey, typename TValue>
class HashMap : public MapCollection<TKey, TValue>
{
	/*
		[�ذ��ؾ��� ����]
		1. �� ��Ŷ�� �ƿ� �˻����� �ʰ� ��� Ű�°� �������� ������?
			-> �𸣰ڴ�.
			-> ��ø� �̰� ��
		2. ��� ���ͷ������� �ؾ��ұ�?
	*/

	using THasher			= typename Hasher<TKey>;
	using TBucket			= typename Bucket<TKey, TValue>;
	using TBucketNode		= typename BucketNode<TKey, TValue>;
	using TMapCollection	= typename MapCollection<TKey, TValue>;
	using TKeyValuePair		= typename KeyValuePair<TKey, TValue>;
	using TIterator			= typename Iterator<TKeyValuePair>;
	using THashMap			= typename HashMap<TKey, TValue>;
	using THashMapIterator  = typename HashMapIterator<TKey, TValue>;
public:
	HashMap(int capacity = ms_iTableDefaultCapacity) : TMapCollection() {
		m_pTable = new TBucket[capacity];

		m_pHeadBucket = &_ValtyHead;
		m_pTailBucket = &_ValtyTail;

		m_pHeadBucket->Previous = nullptr;
		m_pTailBucket->Next = nullptr;

		m_iCapacity = capacity;

		ConnectBucket(m_pHeadBucket, m_pTailBucket);
	}

	virtual ~HashMap() noexcept {
		Clear();

		delete[] m_pTable;
	}
public:
	virtual void Insert(const TKeyValuePair& pair) {
		if (IsFull()) {
			Expand(m_iCapacity * ms_iBucketExpandingFactor);
		}

		if (Find(pair.Key)) {
			throw InvalidArgumentException("�ش� Ű���� �����Ͱ� �̹� �����մϴ�.");
		}

		Int32U uiHash = Hash(pair.Key);
		Int32U uiBucket = Bucket(uiHash);

		if (m_pTable[uiBucket].IsEmpty()) {
			InsertBucketPrev(m_pTailBucket, &m_pTable[uiBucket]);
		} 

		m_pTable[uiBucket].Nodes.PushBack({ { pair.Key, pair.Value }, uiHash });
		
		this->m_iSize++;
	}

	virtual void Insert(TKeyValuePair&& pair) {
		Insert(pair);
	}

	virtual bool Find(const TKey& key) const {
		return m_pTable[HashBucket(key)].Find(key);
	}
	virtual TValue& Get(const TKey& key) const {
		return m_pTable[HashBucket(key)].Get(key);
	}
	virtual bool Remove(const TKey& key) {
		return m_pTable[HashBucket(key)].Remove(key);
	}

	virtual void Clear() {

	}

	void PrintInfo() {
		for (int i = 0; i < m_iCapacity; i++) {
			if (m_pTable[i].IsEmpty()) {
				continue;
			}

			printf("[%d] ������ �� : %d\n", i, m_pTable[i].Size());

			auto it = m_pTable[i].Nodes.Begin();

			while (it->HasNext()) {
				auto v = it->Next();
				printf("\tŰ : %d, �� : %d\n", v.Pair.Key, v.Pair.Value);
			}
		}
	}

	virtual SharedPointer<TIterator> Begin() const {
		return MakeShared<THashMapIterator>(this->GetOwner(), m_pHeadBucket->Next);
	}

	virtual SharedPointer<TIterator> End() const {
		return MakeShared<THashMapIterator>(this->GetOwner(), m_pTailBucket);
	}
protected:
	void Expand(int capacity) {

	}

	void ConnectBucket(TBucket* lhs, TBucket* rhs) {
		lhs->Next = rhs;
		rhs->Previous = lhs;
	}

	/// <summary>
	/// bucket �ٷ� ���� �ٸ� bucket�� �����Ѵ�.
	/// </summary>
	/// <param name="tailPrev"></param>
	/// <param name="newNode"></param>
	void InsertBucketPrev(TBucket* bucket, TBucket* otherBucket) {
		if (bucket == m_pHeadBucket) {
			throw InvalidArgumentException("��� �������� ��带 �����ϸ� �ȵǿ�!");
		}

		/*
			[���� ��]
			 �� <=> �� <=> �� <=> �� <=> �� <=> ��
									 ��     ��
										  node
								  node->prev

			[���� ��]
			 �� <=> �� <=> �� <=> �� <=> �� <=> �� <=> ��
									 ��     ��     ��
								 node->prev     node
										 newNode
		 */

		TBucket* pBucketPrev = bucket->Previous;

		ConnectBucket(pBucketPrev, otherBucket);
		ConnectBucket(otherBucket, bucket);
	}


	Int32U Bucket(const Int32U hash) const {
		return hash % m_iCapacity;
	}

	Int32U Hash(const TKey& key) const {
		return THasher()(key);
	}

	Int32U HashBucket(const TKey& key) const {
		return Bucket(Hash(key));
	}

	bool IsFull() const {
		return this->m_iSize == m_iCapacity;
	}
	
	static constexpr Int32U	ms_iBucketExpandingFactor = 8;	// ���̺� ũ�⸸ŭ �����Ͱ� ���� Ȯ���ϴµ� ��質 Ȯ���� ��
	static constexpr Int32U	ms_iTableDefaultCapacity = 16;	// ���̺� �ʱ� ũ��
protected:
	TBucket* m_pTable;
	TBucket* m_pHeadBucket;
	TBucket* m_pTailBucket;
	Int32U m_iCapacity;

private:
	TBucket _ValtyHead;		// �Ӹ� ���̳��
	TBucket _ValtyTail;		// ���� ���̳��

	friend class THashMapIterator;
};

} // namespace JCore
