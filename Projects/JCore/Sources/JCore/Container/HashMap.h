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
	using TKeyValuePair	 = KeyValuePair<TKey, TValue>;
	using TBucketNode	 = BucketNode<TKey, TValue>;

	bool operator==(const TBucketNode& other) {
		return Pair == other.Pair && Hash == other.Hash;
	}

	// LinkedList 더미 헤드/테일 생성시 필요한 디폴트 생성자
	BucketNode() = default; 
	~BucketNode() noexcept = default;
	BucketNode(const TBucketNode& other) = default;
	BucketNode(TBucketNode&& other) = default;
	BucketNode(const TKeyValuePair& pair, const Int32U hash) : Pair(pair), Hash(hash) {}
	BucketNode(TKeyValuePair&& pair, const Int32U hash) : Pair(Move(pair)), Hash(hash) {}
	BucketNode& operator=(const BucketNode& other) = default;
	BucketNode& operator=(BucketNode&& other) = default;

	TKeyValuePair Pair;
	Int32U Hash{};				// 처음에 한번 계산해놓으면 성능이 좀더 개선될 듯?
};

template <typename TKey, typename TValue>
struct Bucket : public LinkedList<BucketNode<TKey, TValue>>
{
private:
	using TBucketNode = BucketNode<TKey, TValue>;
	using TLinkedList = LinkedList<BucketNode<TKey, TValue>>;
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
	/* =====================================================================
	 *
	 *
	 *		[해결해야할 문제]
	 *		1. 빈 버킷을 아예 검사하지 않고 어떻게 키셋과 벨류셋을 얻을까?
	 *			-> 모르겠다.
	 *			-> 잠시만 이거 와 방법 생각남
	 *			-> 검색 도움 없이 해결완료
	 *		2. 어떻게 이터레이팅을 해야할까?
	 *			-> 검색 도움 없이 해결완료
	 *
	 *
	 * ==================================================================== */

	using THasher					= Hasher<TKey>;
	using TBucket					= Bucket<TKey, TValue>;
	using TBucketNode				= BucketNode<TKey, TValue>;
	using TMapCollection			= MapCollection<TKey, TValue>;
	using TKeyValuePair				= KeyValuePair<TKey, TValue>;
	using TIterator					= Iterator<TKeyValuePair>;
	using THashMap					= HashMap<TKey, TValue>;
	using THashMapIterator			= HashMapIterator<TKey, TValue>;
	using TKeyCollection			= typename TMapCollection::KeyCollection;
	using TValueCollection			= typename TMapCollection::ValueCollection;
	using TKeyCollectionIterator	= typename TMapCollection::KeyCollectionIterator;
	using TValueCollectionIterator	= typename TMapCollection::ValueCollectionIterator;
public:
	// 내부 구조체 전방 선언 (inner struct forward declaration)
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

	HashMap(THashMap&& other) noexcept
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

	~HashMap() noexcept override {
		Clear();
		DeleteArraySafe(m_pTable);
	}
public:


	THashMap& operator=(const THashMap& other) {

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

		// Head 버킷과 Tail 버킷 사이에 연결된 버킷이 있는 경우에만 연결해줘야함
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


	template <typename Ky, typename Vy>
	bool Insert(Ky&& key, Vy&& value) {
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

		
		/*
		 *  [문제 발생]
		 *	m_pTable[uiBucket].PushBack({ { Forward<Ky>(key), Forward<Vy>(value)}, uiHash });
		 *	첨에 이렇게 구현했는데 메모리릭 발생한다. ㄷㄷㄷ
		 *  HashMap<int, int> 같은 원시타입은 해제가 잘 되는데 HashMap<int, String> 과 같은 녀석이 문자열은 동적할당이 해제가 안되고 있었다.
		 *
		 *
		 *  [정도의 고민과 해결 과정]
		 *	내가 구현한 ListCollection.h의 함수에서 ListNode를 new TListNode로 생성하면서 Placement Allocation을 생성자에서 수행해버린다.
		 *
		 *	TListNode* CreateNewNode(const T& data) {
		 *		TListNode* pNewNode = new TListNode;			--> 여기서 Placement Allocation을 수행해버린다.
		 *		pNewNode->Value = data;							--> 이때 이걸로 덮어써버리는데 new TListNoed를 하면서 Placement Allocate한 
		 *		return pNewNode;									녀석을 해제해줘야하는데 덮어 써버렸기 땜에 이제 해제할 수가 없게됨
		 *	}
		 *
		 *  그래서 아래와 같은 연결리스트를 만들면 오류가 발생한다.
		 * 	LinkedList<Tuple<int, String>> list;
		 * 	list.PushBack({ 1, "abfd" });		// 메모리릭 32bytes
		 * 	list.EmplaceBack( 1, "abfd" );		// 메모리릭 없음
		 * 										   이건 EmplaceBack 함수에서 new TListNode{Forward<Args>(args)...}로 바로 Placement Allocate를 수행하기 때문에
		 *	
		 *
		 *	m_pTable[uiBucket].EmplaceBack({ Forward<Ky>(key), Forward<Vy>(value) }, uiHash);
		 *	그래서 이렇게 수정했는데 이러면 {}이걸 Initalizer_list로 컴파일러가 해석하나보다. 그래서
		 *
		 *  근데 일단 LinkedList<Tuple<int, String>>과 같은 문제를 해결해야한다.
		 *	ListNode에 단순히 PlacementAllocation을 사용할지 안할지를 확인하는 변수를 생성자로 추가로 받을 수 있도록 했다.
		 *
		 *	이렇게 수정하면 일단 동작은 잘했다.
		 *  m_pTable[uiBucket].EmplaceBack(TKeyValuePair{ Forward<Ky>(key), Forward<Vy>(value) }, uiHash);
		 *
		 *  근본적으로 해결하기 위해서
		 *	이렇게 인자로 전달해줘서 복사 or 이동 생성하도록 변경함
		 *   TListNode* CreateNewNode(const T& data) {
		 * 	 	TListNode* pNewNode = new TListNode(data);	
		 *	 	return pNewNode;						
		 *	 }
		 *
		 *	 이제 메모리릭 발생안함
		 *	 LinkedList<Tuple<int, String>> list;
		 *	 list.PushBack({ 1, "abfd" });			 
		 *	 list.EmplaceBack( 1, "abfd" );		
		 */

		
		m_pTable[uiBucket].EmplaceBack(TKeyValuePair{ Forward<Ky>(key), Forward<Vy>(value) }, uiHash);
		this->m_iSize++;
		return true;
	}

	bool Insert(const TKeyValuePair& pair) override {
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

	bool Insert(TKeyValuePair&& pair) override {
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

	bool Exist(const TKey& key) const override {
		return m_pTable[HashBucket(key)].ExistByKey(key);
	}

	virtual TValue* Find(const TKey& key) const {
		TValue* pVal = m_pTable[HashBucket(key)].FindByKey(key);

		if (pVal == nullptr) {
			return nullptr;
		}

		return pVal;
	}

	TValue& Get(const TKey& key) const override {
		TValue* pVal = m_pTable[HashBucket(key)].FindByKey(key);

		if (pVal == nullptr) {
			throw InvalidArgumentException("해당 키값에 대응하는 값이 존재하지 않습니다.");
		}

		return *pVal;
	}

	bool Remove(const TKey& key) override {
		TBucket& bucket = m_pTable[HashBucket(key)];
		if (!bucket.RemoveByKey(key)) {
			return false;
		}

		// 버킷이 비었으면 연결을 끊어준다.
		if (bucket.IsEmpty()) {
			ConnectBucket(bucket.Previous, bucket.Next);
		}
	
		this->m_iSize--;
		return true;
	}

	void Clear() override {
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

			printf("[%d] 데이터 수 : %d\n", i, m_pTable[i].Size());

			auto it = m_pTable[i].Begin();

			while (it->HasNext()) {
				auto v = it->Next();
				printf("\t키 : %d, 값 : %d\n", v.Pair.Key, v.Pair.Value);
			}
		}
	}

	SharedPointer<TIterator> Begin() const override {
		return MakeShared<THashMapIterator>(this->GetOwner(), m_pHeadBucket->Next, m_pHeadBucket->Next->m_pHead->Next);
	}

	SharedPointer<TIterator> End() const override {
		return MakeShared<THashMapIterator>(this->GetOwner(), m_pTailBucket->Previous, m_pTailBucket->Previous->m_pTail);
	}

	TKeyCollection& Keys() override {
		return m_KeyCollection;
	}

	TValueCollection& Values() override {
		return m_ValueCollection;
	}
protected:
	void Expand(int capacity) {
		TBucket* pNewTable = new TBucket[capacity];
		const int iPrevCapacity = m_iCapacity;

		m_iCapacity = capacity;

		ConnectBucket(m_pHeadBucket, m_pTailBucket);

		for (int i = 0; i < iPrevCapacity; i++) {
			if (m_pTable[i].Size() == 0) {
				continue;
			}

			TBucket& prevBucket = m_pTable[i];
			auto prevBucketIterator = prevBucket.Begin();

			// 기존 버킷을 순회하며 새로운 버킷에 데이터를 담아준다.
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

		const int iCapacity = CalculateExpandCapacity(size);
		Expand(iCapacity);
		return true;
	}

	/// <summary>
	/// 전달받은 사이즈 크기에 맞는 배열 크기를 반환해준다.
	/// </summary>
	int CalculateExpandCapacity(int size) const {
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

	static void ConnectBucket(TBucket* lhs, TBucket* rhs) {
		lhs->Next = rhs;
		rhs->Previous = lhs;
	}

	/// <summary>
	/// bucket 바로 전에 다른 bucket을 삽입한다.
	/// </summary>
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
	
	static constexpr Int32U	ms_iTableExpandingFactor = 8;	// 테이블 크기만큼 데이터가 들어가면 확장하는데 몇배나 확장할 지
	static constexpr Int32U	ms_iTableDefaultCapacity = 16;	// 테이블 초기 크기
protected:
	TBucket* m_pTable = nullptr;
	TBucket* m_pHeadBucket = &m_ValtyHead;
	TBucket* m_pTailBucket = &m_ValtyTail;
	Int32U m_iCapacity = 0;

	HashMapKeyCollection m_KeyCollection;
	HashMapValueCollection m_ValueCollection;
private:
	TBucket m_ValtyHead;		// 머리 더미노드
	TBucket m_ValtyTail;		// 꼬리 더미노드
public:
	struct HashMapKeyCollection : public TKeyCollection
	{
		using TEnumerator		= SharedPointer<Iterator<TKey>>;
		using TCollection		= Collection<TKey>;

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

		virtual ~HashMapKeyCollection() noexcept override = default;

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

	struct HashMapKeyCollectionIterator final : public TKeyCollectionIterator
	{
		using TListNode = ListNode<BucketNode<TKey, TValue>>;

		HashMapKeyCollectionIterator(VoidOwner& owner, TBucket* currentBucket, TListNode* currentNode)
			: m_HashMapIterator(owner, currentBucket, currentNode), TKeyCollectionIterator(owner, &m_HashMapIterator)
		{
		}
		virtual ~HashMapKeyCollectionIterator() noexcept = default;

		THashMapIterator m_HashMapIterator;
	};

	struct HashMapValueCollection final : public TValueCollection
	{
		using TEnumerator		= SharedPointer<Iterator<TValue>>;
		using TCollection		= Collection<TValue>;

		HashMapValueCollection(THashMap* hashMap)
			: TMapCollection::ValueCollection(hashMap, ContainerType::HashMapValueCollection)
		{
			m_pHashMap = hashMap;
		}

		virtual ~HashMapValueCollection() noexcept override = default;

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

	struct HashMapValueCollectionIterator final : public TValueCollectionIterator
	{
		using TListNode = ListNode<BucketNode<TKey, TValue>>;

		HashMapValueCollectionIterator(VoidOwner& owner, TBucket* currentBucket, TListNode* currentNode)
			: m_HashMapIterator(owner, currentBucket, currentNode), TValueCollectionIterator(owner, &m_HashMapIterator)
		{
		}

		virtual ~HashMapValueCollectionIterator() noexcept override = default;

		THashMapIterator m_HashMapIterator;
	};


	friend class THashMapIterator;

}; // class HashMap<TKey, TValue>

} // namespace JCore

