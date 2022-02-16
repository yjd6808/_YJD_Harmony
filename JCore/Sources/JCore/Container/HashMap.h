/*
	작성자 : 윤정도
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
	Int32U Hash;				// 처음에 한번 계산해놓으면 편할 듯?
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
		[해결해야할 문제]
		1. 빈 버킷을 아예 검사하지 않고 어떻게 키셋과 벨류셋을 얻을까?
			-> 모르겠다.
			-> 잠시만 이거 와
		2. 어떻게 이터레이팅을 해야할까?
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
			throw InvalidArgumentException("해당 키값의 데이터가 이미 존재합니다.");
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

			printf("[%d] 데이터 수 : %d\n", i, m_pTable[i].Size());

			auto it = m_pTable[i].Nodes.Begin();

			while (it->HasNext()) {
				auto v = it->Next();
				printf("\t키 : %d, 값 : %d\n", v.Pair.Key, v.Pair.Value);
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
	/// bucket 바로 전에 다른 bucket을 삽입한다.
	/// </summary>
	/// <param name="tailPrev"></param>
	/// <param name="newNode"></param>
	void InsertBucketPrev(TBucket* bucket, TBucket* otherBucket) {
		if (bucket == m_pHeadBucket) {
			throw InvalidArgumentException("헤드 이전에는 노드를 삽입하면 안되요!");
		}

		/*
			[삽입 전]
			 ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■
									 ↑     ↑
										  node
								  node->prev

			[삽입 후]
			 ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■
									 ↑     ↑     ↑
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
	
	static constexpr Int32U	ms_iBucketExpandingFactor = 8;	// 테이블 크기만큼 데이터가 들어가면 확장하는데 몇배나 확장할 지
	static constexpr Int32U	ms_iTableDefaultCapacity = 16;	// 테이블 초기 크기
protected:
	TBucket* m_pTable;
	TBucket* m_pHeadBucket;
	TBucket* m_pTailBucket;
	Int32U m_iCapacity;

private:
	TBucket _ValtyHead;		// 머리 더미노드
	TBucket _ValtyTail;		// 꼬리 더미노드

	friend class THashMapIterator;
};

} // namespace JCore
