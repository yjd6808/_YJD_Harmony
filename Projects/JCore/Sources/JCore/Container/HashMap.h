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
	using TKeyValuePair	 = typename KeyValuePair<TKey, TValue>;
	using TBucketNode	 = typename BucketNode<TKey, TValue>;

	bool operator==(const TBucketNode& other) {
		return Pair == other.Pair && Hash == other.Hash;
	}
	
	

	/*
	�׳� ����Ʈ ���� �� ��?
	BucketNode() {}
	~BucketNode() {}

	BucketNode& operator=(const BucketNode& other) {
		Hash = other.Hash;
		Pair.Key = other.Pair.Key;
		Pair.Value = other.Pair.Value;
		return *this;
	}

	BucketNode& operator=(BucketNode&& other) {
		Hash = other.Hash;
		Pair.Key = Move(other.Pair.Key);
		Pair.Value = Move(other.Pair.Value);
		return *this;
	}
	*/

	BucketNode() = default; // LinkedList ���� ���/���� ������ �ʿ��� ����Ʈ ������
	~BucketNode() noexcept = default;
	BucketNode(const TBucketNode& other) = default;
	BucketNode(TBucketNode&& other) = default;
	BucketNode(const TKeyValuePair& pair, const Int32U hash) : Pair(pair), Hash(hash) {}
	BucketNode(TKeyValuePair&& pair, const Int32U hash) : Pair(Move(pair)), Hash(hash) {}
	BucketNode& operator=(const BucketNode& other) = default;
	BucketNode& operator=(BucketNode&& other) = default;

	TKeyValuePair Pair;
	Int32U Hash;				// ó���� �ѹ� ����س����� ���� ��?
};

template <typename TKey, typename TValue>
struct Bucket : public LinkedList<BucketNode<TKey, TValue>>
{
private:
	using TBucketNode = typename BucketNode<TKey, TValue>;
	using TLinkedList = typename LinkedList<BucketNode<TKey, TValue>>;
public:
	bool ExistByKey(const TKey& key) {

		return TLinkedList::Extension().ExistIf([&key](const TBucketNode& node) { return node.Pair.Key == key; });
	}

	TValue* FindByKey(const TKey& key) {
		TBucketNode* pFind = TLinkedList::Extension().FindIf([&key](const TBucketNode& node) { return node.Pair.Key == key; });

		if (pFind == nullptr) {
			return nullptr;
		}

		return AddressOf(pFind->Pair.Value);
	}

	bool RemoveByKey(const TKey& key) {
		return TLinkedList::RemoveIf([&key](const TBucketNode& node) { return node.Pair.Key == key; });
	}

	Bucket* Next = nullptr;
	Bucket* Previous = nullptr;

	friend class HashMap<TKey, TValue>;
};

template <typename TKey, typename TValue>
class HashMap : public MapCollection<TKey, TValue>
{
	/*
		[�ذ��ؾ��� ����]
		1. �� ��Ŷ�� �ƿ� �˻����� �ʰ� ��� Ű�°� �������� ������?
			-> �𸣰ڴ�.
			-> ��ø� �̰� �� ��� ������
			-> �˻� ���� ���� �ذ�Ϸ�
		2. ��� ���ͷ������� �ؾ��ұ�?
			-> �˻� ���� ���� �ذ�Ϸ�
	*/

	using THasher					= typename Hasher<TKey>;
	using TBucket					= typename Bucket<TKey, TValue>;
	using TBucketNode				= typename BucketNode<TKey, TValue>;
	using TMapCollection			= typename MapCollection<TKey, TValue>;
	using TKeyValuePair				= typename KeyValuePair<TKey, TValue>;
	using TIterator					= typename Iterator<TKeyValuePair>;
	using THashMap					= typename HashMap<TKey, TValue>;
	using THashMapIterator			= typename HashMapIterator<TKey, TValue>;
	using TKeyCollection			= typename TMapCollection::KeyCollection;
	using TValueCollection			= typename TMapCollection::ValueCollection;
	using TKeyCollectionIterator	= typename TMapCollection::KeyCollectionIterator;
	using TValueCollectionIterator	= typename TMapCollection::ValueCollectionIterator;
public:
	// ���� ����ü ���� ���� (inner struct forward declaration)
	struct HashMapKeyCollection;
	struct HashMapKeyCollectionIterator;
	struct HashMapValueCollection;
	struct HashMapValueCollectionIterator;
public:
	HashMap(int capacity = ms_iTableDefaultCapacity) 
		: TMapCollection(ContainerType::HashMap),
		m_KeyCollection(this),
		m_ValueCollection(this)
	{
		m_pTable = new TBucket[capacity];
		m_pHeadBucket->Previous = nullptr;
		m_pTailBucket->Next = nullptr;

		m_iCapacity = capacity;

		ConnectBucket(m_pHeadBucket, m_pTailBucket);
	}

	HashMap(const THashMap& other)
		: THashMap(other.m_iCapacity) 
	{
		operator=(other);
	}

	HashMap(THashMap&& other)
		: TMapCollection(ContainerType::HashMap),
		m_KeyCollection(this),
		m_ValueCollection(this)
	{
		operator=(Move(other));
	}

	HashMap(std::initializer_list<TKeyValuePair> ilist)
		: THashMap(ilist.size())
	{
		operator=(ilist);
	}

	virtual ~HashMap() noexcept {
		Clear();
		DeleteArraySafe(m_pTable);
	}
public:
	//Lazy Template Method Instantiation�� ����.. ���ø����� ó��
	//template <typename UKey, typename UValue>
	THashMap& operator=(const THashMap& other) {
		// static_assert(IsSameType_v<UKey, TKey>, "... UKey and TKey is difference type.");
		// static_assert(IsSameType_v<UValue, TValue>, "... UValue and TValue is difference type.");
		// static_assert(IsAssignable_v<UKey, TKey>, "... UKey cannot be assign to TKey. (UKey = TKey is impossible operation)");
		// static_assert(IsAssignable_v<UValue, TValue>, "... UValue cannot be assign to TValue. (UValue = TValue is impossible operation)");

		// using UBucket	= Bucket<UKey, UValue>;
		// using UListNode  = ListNode<BucketNode<UKey, UValue>>;

		Clear();
		ExpandIfNeeded(other.m_iSize);

		TBucket* pOtherBucketCur = other.m_pHeadBucket->Next;
		while (pOtherBucketCur != other.m_pTailBucket) {
			auto nodeIterator = pOtherBucketCur->Begin();
			while (nodeIterator->HasNext()) {
				TBucketNode& node = nodeIterator->Next();
				Int32U uiBucket = BucketIndex(node.Hash);
				m_pTable[uiBucket].PushBack(node);
			}

			pOtherBucketCur = pOtherBucketCur->Next;
		}

		this->m_iSize = other.m_iSize;
		return *this;
	}

	THashMap& operator=(THashMap&& other) {
		Clear();
		DeleteArraySafe(m_pTable);

		this->m_Owner = Move(other.m_Owner);
		this->m_iSize = other.m_iSize;
		this->m_iCapacity = other.m_iCapacity;
		this->m_pTable = other.m_pTable;
		this->m_KeyCollection = other.m_KeyCollection;
		this->m_ValueCollection = other.m_ValueCollection;

		// Head ��Ŷ�� Tail ��Ŷ ���̿� ����� ��Ŷ�� �ִ� ��쿡�� �����������
		if (this->m_iSize > 0) {
			ConnectBucket(m_pHeadBucket, other.m_pHeadBucket->Next);
			ConnectBucket(other.m_pTailBucket->Previous, m_pTailBucket);
		}

		other.m_pTable = nullptr;
		other.m_iSize = 0;

		return *this;
	}

	THashMap& operator=(std::initializer_list<TKeyValuePair> ilist) {
		Clear();
		ExpandIfNeeded(ilist.size());

		for (auto it = ilist.begin(); it != ilist.end(); it++) {
			Insert((*it));
		}

		return *this;
	}

	TValue& operator[](const TKey& key) {
		return Get(key);
	}


	template <typename _Ky, typename _Vy>
	bool Insert(_Ky&& key, _Vy&& value) {
		if (IsFull()) {
			Expand(m_iCapacity * ms_iTableExpandingFactor);
		}

		if (Exist(key)) {
			return false;
		}

		Int32U uiHash = Hash(key);
		Int32U uiBucket = BucketIndex(uiHash);

		if (m_pTable[uiBucket].IsEmpty()) {
			InsertBucketPrev(m_pTailBucket, &m_pTable[uiBucket]);
		}

		m_pTable[uiBucket].PushBack({ { Forward<_Ky>(key), Forward<_Vy>(value)}, uiHash });
		this->m_iSize++;
	}

	virtual bool Insert(const TKeyValuePair& pair) {
		if (IsFull()) {
			Expand(m_iCapacity * ms_iTableExpandingFactor);
		}

		if (Exist(pair.Key)) {
			return false;
		}

		Int32U uiHash = Hash(pair.Key);
		Int32U uiBucket = BucketIndex(uiHash);

		if (m_pTable[uiBucket].IsEmpty()) {
			InsertBucketPrev(m_pTailBucket, &m_pTable[uiBucket]);
		} 

		m_pTable[uiBucket].EmplaceBack(pair, uiHash);
		this->m_iSize++;
		return true;
	}

	virtual bool Insert(TKeyValuePair&& pair) {
		if (IsFull()) {
			Expand(m_iCapacity * ms_iTableExpandingFactor);
		}

		if (Exist(pair.Key)) {
			return false;
		}

		Int32U uiHash = Hash(pair.Key);
		Int32U uiBucket = BucketIndex(uiHash);

		if (m_pTable[uiBucket].IsEmpty()) {
			InsertBucketPrev(m_pTailBucket, &m_pTable[uiBucket]);
		}

		m_pTable[uiBucket].EmplaceBack(Move(pair), uiHash);
		this->m_iSize++;
		return true;
	}

	virtual bool Exist(const TKey& key) const {
		return m_pTable[HashBucket(key)].ExistByKey(key);
	}

	virtual TValue* Find(const TKey& key) const {
		TValue* pVal = m_pTable[HashBucket(key)].FindByKey(key);

		if (pVal == nullptr) {
			return nullptr;
		}

		return pVal;
	}

	virtual TValue& Get(const TKey& key) const {
		TValue* pVal = m_pTable[HashBucket(key)].FindByKey(key);

		if (pVal == nullptr) {
			throw InvalidArgumentException("�ش� Ű���� �����ϴ� ���� �������� �ʽ��ϴ�.");
		}

		return *pVal;
	}

	virtual bool Remove(const TKey& key) {
		TBucket& bucket = m_pTable[HashBucket(key)];
		if (!bucket.RemoveByKey(key)) {
			return false;
		}

		// ��Ŷ�� ������� ������ �����ش�.
		if (bucket.IsEmpty()) {
			ConnectBucket(bucket.Previous, bucket.Next);
		}
	
		this->m_iSize--;
		return true;
	}

	virtual void Clear() {
		if (this->m_iSize == 0) {
			return;
		}

		TBucket* pCur = m_pHeadBucket->Next;
		while (pCur != m_pTailBucket) {
			pCur->Clear();
			pCur = pCur->Next;
		}

		this->m_iSize = 0;

		ConnectBucket(m_pHeadBucket, m_pTailBucket);
	}

	void PrintInfo() {
		for (int i = 0; i < m_iCapacity; i++) {
			if (m_pTable[i].IsEmpty()) {
				continue;
			}

			printf("[%d] ������ �� : %d\n", i, m_pTable[i].Size());

			auto it = m_pTable[i].Begin();

			while (it->HasNext()) {
				auto v = it->Next();
				printf("\tŰ : %d, �� : %d\n", v.Pair.Key, v.Pair.Value);
			}
		}
	}

	virtual SharedPointer<TIterator> Begin() const {
		return MakeShared<THashMapIterator>(this->GetOwner(), m_pHeadBucket->Next, m_pHeadBucket->Next->m_pHead->Next);
	}

	virtual SharedPointer<TIterator> End() const {
		return MakeShared<THashMapIterator>(this->GetOwner(), m_pTailBucket->Previous, m_pTailBucket->Previous->m_pTail);
	}

	virtual TKeyCollection& Keys() {
		return m_KeyCollection;
	}

	virtual TValueCollection& Values() {
		return m_ValueCollection;
	}
protected:
	void Expand(int capacity) {
		TBucket* pNewTable = new TBucket[capacity];
		int iPrevCapacity = m_iCapacity;

		m_iCapacity = capacity;

		ConnectBucket(m_pHeadBucket, m_pTailBucket);

		for (int i = 0; i < iPrevCapacity; i++) {
			if (m_pTable[i].Size() == 0) {
				continue;
			}

			TBucket& prevBucket = m_pTable[i];
			auto prevBucketIterator = prevBucket.Begin();

			// ���� ��Ŷ�� ��ȸ�ϸ� ���ο� ��Ŷ�� �����͸� ����ش�.
			while (prevBucketIterator->HasNext()) {
				TBucketNode& bucketNode = prevBucketIterator->Next();
				Int32U uiBucket = BucketIndex(bucketNode.Hash);

				if (pNewTable[uiBucket].IsEmpty()) {
					InsertBucketPrev(m_pTailBucket, &pNewTable[uiBucket]);
				}

				pNewTable[uiBucket].PushBack(Move(bucketNode));
			}
		}

		DeleteArraySafe(m_pTable);
		m_pTable = pNewTable;
	}

	bool ExpandIfNeeded(int size) {
		if (size < m_iCapacity) {
			return false;
		}

		int iCapacity = CalculateExpandCapacity(size);
		Expand(iCapacity);
		return true;
	}

	/// <summary>
	/// ���޹��� ������ ũ�⿡ �´� �迭 ũ�⸦ ��ȯ���ش�.
	/// </summary>
	int CalculateExpandCapacity(int size) {
		if (size < m_iCapacity) {
			return m_iCapacity;
		}

		int iExpectedCapacity = m_iCapacity;

		while (true) {
			iExpectedCapacity *= ms_iTableExpandingFactor;
			if (iExpectedCapacity > size) {
				break;
			}
		}
		return iExpectedCapacity;
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


	Int32U BucketIndex(const Int32U hash) const {
		return hash % m_iCapacity;
	}

	Int32U Hash(const TKey& key) const {
		return THasher()(key);
	}

	Int32U HashBucket(const TKey& key) const {
		return BucketIndex(Hash(key));
	}

	bool IsFull() const {
		return this->m_iSize == m_iCapacity;
	}
	
	static constexpr Int32U	ms_iTableExpandingFactor = 8;	// ���̺� ũ�⸸ŭ �����Ͱ� ���� Ȯ���ϴµ� ��質 Ȯ���� ��
	static constexpr Int32U	ms_iTableDefaultCapacity = 16;	// ���̺� �ʱ� ũ��
protected:
	TBucket* m_pTable = nullptr;
	TBucket* m_pHeadBucket = &_ValtyHead;
	TBucket* m_pTailBucket = &_ValtyTail;
	Int32U m_iCapacity = 0;

	HashMapKeyCollection m_KeyCollection;
	HashMapValueCollection m_ValueCollection;
private:
	TBucket _ValtyHead;		// �Ӹ� ���̳��
	TBucket _ValtyTail;		// ���� ���̳��
public:
	struct HashMapKeyCollection : public TKeyCollection
	{
		using TEnumerator		= typename SharedPointer<Iterator<TKey>>;
		using TCollection		= typename Collection<TKey>;

		HashMapKeyCollection(THashMap* hashMap) 
			: TKeyCollection(hashMap, ContainerType::HashMapKeyCollection)
		{
			m_pHashMap = hashMap;
		}

		HashMapKeyCollection& operator=(const HashMapKeyCollection& other) {
			this->m_pHashMap = other.m_pHashMap;
			this->m_pMap = other.m_pHashMap;
			return *this;
		}

		virtual ~HashMapKeyCollection() noexcept = default;

		int Size() const override {
			return TKeyCollection::Size();
		}

		bool IsEmpty() const override {
			return TKeyCollection::IsEmpty();
		}

		TEnumerator Begin() const override {
			return MakeShared<HashMapKeyCollectionIterator>(
				m_pHashMap->GetOwner(), 
				m_pHashMap->m_pHeadBucket->Next,
				m_pHashMap->m_pHeadBucket->Next->m_pHead->Next
			);
		}

		TEnumerator End() const override {
			return MakeShared<HashMapKeyCollectionIterator>(
				m_pHashMap->GetOwner(), 
				m_pHashMap->m_pTailBucket->Previous,
				m_pHashMap->m_pTailBucket->Previous->m_pTail
			);
		}

		THashMap* m_pHashMap;
	};

	struct HashMapKeyCollectionIterator : public TKeyCollectionIterator
	{
		using TListNode = typename ListNode<BucketNode<TKey, TValue>>;

		HashMapKeyCollectionIterator(VoidOwner& owner, TBucket* currentBucket, TListNode* currentNode)
			: m_HashMapIterator(owner, currentBucket, currentNode), TKeyCollectionIterator(owner, &m_HashMapIterator)
		{
		}
		virtual ~HashMapKeyCollectionIterator() noexcept = default;

		THashMapIterator m_HashMapIterator;
	};

	struct HashMapValueCollection : public TValueCollection
	{
		using TEnumerator		= typename SharedPointer<Iterator<TValue>>;
		using TCollection		= typename Collection<TValue>;

		HashMapValueCollection(THashMap* hashMap)
			: TMapCollection::ValueCollection(hashMap, ContainerType::HashMapValueCollection)
		{
			m_pHashMap = hashMap;
		}

		virtual ~HashMapValueCollection() noexcept = default;

		HashMapValueCollection& operator=(const HashMapValueCollection& other) {
			this->m_pHashMap = other.m_pHashMap;
			this->m_pMap = other.m_pHashMap;
			return *this;
		}

		TEnumerator Begin() const override {
			return MakeShared<HashMapValueCollectionIterator>(
				m_pHashMap->GetOwner(),
				m_pHashMap->m_pHeadBucket->Next,
				m_pHashMap->m_pHeadBucket->Next->m_pHead->Next
			);
		}

		TEnumerator End() const override {
			return MakeShared<HashMapValueCollectionIterator>(
				m_pHashMap->GetOwner(),
				m_pHashMap->m_pTailBucket->Previous,
				m_pHashMap->m_pTailBucket->Previous->m_pTail
			);
		}

		THashMap* m_pHashMap;
	};

	struct HashMapValueCollectionIterator : public TValueCollectionIterator
	{
		using TListNode = typename ListNode<BucketNode<TKey, TValue>>;

		HashMapValueCollectionIterator(VoidOwner& owner, TBucket* currentBucket, TListNode* currentNode)
			: m_HashMapIterator(owner, currentBucket, currentNode), TValueCollectionIterator(owner, &m_HashMapIterator)
		{
		}

		virtual ~HashMapValueCollectionIterator() noexcept = default;

		THashMapIterator m_HashMapIterator;
	};


	friend class THashMapIterator;

}; // class HashMap<TKey, TValue>

} // namespace JCore

