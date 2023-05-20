/*
 *	작성자 : 윤정도
 */

#pragma once

#include <JCore/Hasher.h>
#include <JCore/Memory.h>

#include <JCore/Container/MapCollection.h>
#include <JCore/Container/LinkedList.h>
#include <JCore/Container/HashMapIterator.h>

NS_JC_BEGIN

template <typename TKey, typename TValue>
struct BucketNode
{
	using TKeyValuePair	 = Pair<TKey, TValue>;
	using TBucketNode	 = BucketNode<TKey, TValue>;

	bool operator==(const TBucketNode& other) {
		return Pair == other.Pair && Hash == other.Hash;
	}

	
	BucketNode() = default;
	BucketNode(const TKeyValuePair& pair, const Int32U hash)
		: Pair(pair)
		, Hash(hash)
	{}
	
	BucketNode(TKeyValuePair&& pair, const Int32U hash)
		: Pair(Move(pair))
		, Hash(hash)
	{}

	BucketNode(const TBucketNode& other) { this->operator=(other); }
	BucketNode(TBucketNode&& other) { this->operator=(Move(other)); }

	TBucketNode& operator=(const BucketNode& other) {
		Pair.Key = other.Pair.Key;
		Pair.Value = other.Pair.Value;
		Hash = other.Hash;
		return *this;
	}

	TBucketNode& operator=(BucketNode&& other) {
		Pair.Key = Move(other.Pair.Key);		// 이게 엄청 중요하디. 키가 Fundamental 타입이 아닐수가 있다.
		Pair.Value = Move(other.Pair.Value);
		Hash = other.Hash;
		return *this;
	}

	TKeyValuePair Pair;
	Int32U Hash{};	// 처음에 한번 계산해놓으면 성능이 좀더 개선될 듯?
};



/* ==============================================================
 *               
 * 2022/12/25일까지 기존 연결리스트 확장해서 씀
 * 하지만 성능이 안좋고 너무 무겁기 때문에 경량화해서 새로 작성이 필요하다.
 *
 * ============================================================== */
/*
template <typename TKey, typename TValue, typename TAllocator>
struct Bucket : public LinkedList<BucketNode<TKey, TValue>, TAllocator>
{
private:
	using TBucketNode = BucketNode<TKey, TValue>;
	using TLinkedList = LinkedList<BucketNode<TKey, TValue>, TAllocator>;
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

	template <typename, typename, typename> friend class HashMap;
};
*/



/* ==============================================================
 *
 * 12월 25일 일요일 크리스마스 기념! 버킷을 공허의 다이나믹 배열로 변경
 * 공허의 다이나믹 배열?: 데이터를 넣을때 초기화를 진행한다!
 * 보통 1개의 데이터를 넣게 되므로 초기 용량은 1로 설정하고 이후 +2씩 커지도록 하였다.
 * 근데 3개까지 찰 확률은 거의 없다.
 *
 * ============================================================== */
template <typename TKey, typename TValue, typename TAllocator>
struct Bucket
{
	using TBucketNode = BucketNode<TKey, TValue>;

	Bucket()
	: Next(nullptr)
	, Previous(nullptr)
	, DynamicArray(nullptr)
	, Capacity(1)
	, Size(0) {}
	~Bucket() {
		JCORE_PLACEMENT_DELETE_ARRAY_SAFE(DynamicArray, Size);
		JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(DynamicArray, sizeof(TBucketNode) * Capacity);
	}

	template <typename... Args>
	void EmplaceBack(Args&&... args) {
		if (!IsValid())
			Initialize();

		if (IsFull()) 
			Expand(Capacity + 2);

		Memory::PlacementNew(DynamicArray[Size++], Forward<Args>(args)...);
	}

	void PushBack(const TBucketNode& data) {
		if (!IsValid())
			Initialize();

		if (IsFull())
			Expand(Capacity + 2);

		Memory::PlacementNew(DynamicArray[Size++], data);
	}

	void PushBack(TBucketNode&& data) {
		if (!IsValid())
			Initialize();

		if (IsFull())
			Expand(Capacity + 2);

		Memory::PlacementNew(DynamicArray[Size++], Move(data));
	}

	void Initialize() {
		int iAllocated;
		DynamicArray = TAllocator::template Allocate<TBucketNode*>(sizeof(TBucketNode) * Capacity, iAllocated);
		// Memory::Set(DynamicArray, sizeof(TBucketNode) * Capacity, 0);
	}

	void Expand(int newCapacity) {
		int iAllocated;
		TBucketNode* pNewDynamicArray = TAllocator::template Allocate<TBucketNode*>(sizeof(TBucketNode) * newCapacity, iAllocated);

		// 2023/02/23
		// 키가 String 같은 타입인 경우 그냥 대입 해버리면 Key의 생성자로 초기화가 수행이 안되어있기때문에 오류가 발생한다.
		// String의 경우 m_pBuffer가 nullptr로 초기화가 되지 않음
		// 기본 생성자를 호출해서 초기화를 해놔야한다.
		if constexpr (IsPointerType_v<TValue> && !IsFundamentalType_v<TKey> && !IsPointerType_v<TKey>) {
			if constexpr (IsStringType_v<TKey>)
				Memory::PlacementNewArray(pNewDynamicArray, newCapacity, TBucketNode{ { 0, nullptr }, 0 });	// 문자열은 동적할당 안된 상태로 생성해주자. String(0)는 동적할당안함
			else 
				DebugAssert(false); // String도 아니고 int같은 기본 타입도 아닌 새로운 키타입을 추가하고자 한다면 성능향상을 위해 여기서 직접 수정 ㄱ, 그냥 Memory::PlacementNewArray(pNewDynamicArray, newCapacity)를 수행해도 동작하는데 문제없긴함
		}

		for (int i = 0; i < Size; i++) {
			if constexpr (IsPointerType_v<TValue>)
				pNewDynamicArray[i] = Move(DynamicArray[i]);	// 2023/02/23 Move 필수.. Value가 포인터라지만 Key가 String 같은 타입일 수가 있다. 만약 Move를 안하고 Copy를 한다면 DynamicArray의 메모리를 해제하기전에 수동으로 소멸자 호출을 해줘야함.
			else
				Memory::PlacementNew(pNewDynamicArray[i], Move(DynamicArray[i]));
		}

		TAllocator::Deallocate(DynamicArray, sizeof(TBucketNode) * Capacity);
		DynamicArray = pNewDynamicArray;
		Capacity = newCapacity;
	}

	void Clear() {
		JCORE_PLACEMENT_DELETE_ARRAY_SAFE(DynamicArray, Size);
		Size = 0;
	}

	bool ExistByKey(const TKey& key) {
		for (int i = 0; i < Size; i++) {
			if (DynamicArray[i].Pair.Key == key) {
				return true;
			}
		}

		return false;
	}

	TValue* FindByKey(const TKey& key) {
		for (int i = 0; i < Size; i++) {
			if (DynamicArray[i].Pair.Key == key) {
				return AddressOf(DynamicArray[i].Pair.Value);
			}
		}

		return nullptr;
	}

	bool RemoveByKey(const TKey& key) {
		int iFind = -1;
		for (int i = 0; i < Size; i++) {
			if (DynamicArray[i].Pair.Key == key) {
				Memory::PlacementDelete(DynamicArray[i]);
				iFind = i;
				break;
			}
		}

		if (iFind == -1) {
			return false;
		}

		for (int i = iFind; i < Size - 1; i++) {
			DynamicArray[i] = Move(DynamicArray[i + 1]);
		}

		--Size;
		return true;
	}

	bool IsFull() {
		return Size == Capacity;
	}

	bool IsEmpty() {
		return Size == 0;
	}

	bool IsValid() {
		return DynamicArray != nullptr;
	}


	TBucketNode& GetAt(const int idx) {
		DebugAssert(DynamicArray);
		DebugAssert(idx >= 0 && idx < Size);
		return DynamicArray[idx];
	}

	Bucket* Next;
	Bucket* Previous;
	TBucketNode* DynamicArray;
	int Capacity;
	int Size;
};

template <typename TKey, typename TValue, typename TAllocator = DefaultAllocator>
class HashMap : public MapCollection<TKey, TValue, TAllocator>
{
	/* =====================================================================
	 *
	 *
	 *		[해결해야할 문제]
	 *		1. 빈 버킷을 아예 검사하지 않고 어떻게 키셋과 벨류셋을 얻을까?
	 *			-> 모르겠다.
	 *			-> 잠시만 이거 와 방법 생각남
	 *			-> 검색 도움 없이 해결완료
	 *		2. 어떻게 이터레이션을 해야할까?
	 *			-> 검색 도움 없이 해결완료
	 *
	 *
	 * ==================================================================== */
public:
	using THasher					= Hasher<TKey>;
	using TBucket					= Bucket<TKey, TValue, TAllocator>;
	using TBucketNode				= BucketNode<TKey, TValue>;
	using TMapCollection			= MapCollection<TKey, TValue, TAllocator>;
	using TKeyValuePair				= Pair<TKey, TValue>;
	using TIterator					= Iterator<TKeyValuePair, TAllocator>;
	using THashMap					= HashMap<TKey, TValue, TAllocator>;
	using THashMapIterator			= HashMapIterator<TKey, TValue, TAllocator>;
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
		: TMapCollection()
		, m_pHeadBucket(nullptr)
		, m_pTailBucket(nullptr)
		, m_iCapacity(capacity)
	{
		int iAllocatedSize;
		m_pTable = TAllocator::template Allocate<TBucket*>(sizeof(TBucket) * capacity, iAllocatedSize);
		Memory::PlacementNewArray(m_pTable, capacity);
	}

	HashMap(const THashMap& other) : THashMap(other.m_iCapacity) {
		operator=(other);
	}

	HashMap(THashMap&& other) noexcept
		: m_pTable(nullptr)
		, m_pHeadBucket(nullptr)
		, m_pTailBucket(nullptr)
	{
		operator=(Move(other));
	}

	// 이니셜라이저로 초기화 하는 경우 보통 더 확장안시킬 확률이 크므로.. 맞춤형으로 가자.
	HashMap(std::initializer_list<TKeyValuePair> ilist) : THashMap(ilist.size() + 1) {
		operator=(ilist);
	}

	~HashMap() noexcept override {
		THashMap::Clear();

		JCORE_PLACEMENT_DELETE_ARRAY_SAFE(m_pTable, m_iCapacity);
		JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(m_pTable, m_iCapacity * sizeof(TBucket));
	}
public:


	THashMap& operator=(const THashMap& other) {
		THashMap::Clear();
		ExpandIfNeeded(other.m_iSize);

		TBucket* pOtherBucketCur = other.m_pHeadBucket;
		while (pOtherBucketCur != nullptr) {
			for (int i = 0; i < pOtherBucketCur->Size; i++) {
				TBucketNode& node = pOtherBucketCur->GetAt(i);
				Int32U uiBucket = BucketIndex(node.Hash);
				m_pTable[uiBucket].PushBack(node);
			}
			pOtherBucketCur = pOtherBucketCur->Next;
		}

		this->m_iSize = other.m_iSize;
		return *this;
	}

	THashMap& operator=(THashMap&& other) noexcept {
		Clear();
		JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(m_pTable, sizeof(TBucket) * m_iCapacity);

		this->m_Owner = Move(other.m_Owner);
		this->m_iSize = other.m_iSize;
		this->m_iCapacity = other.m_iCapacity;
		this->m_pTable = other.m_pTable;
		this->m_pHeadBucket = other.m_pHeadBucket;
		this->m_pTailBucket = other.m_pTailBucket;;

		other.m_pTable = nullptr;
		other.m_pHeadBucket = nullptr;
		other.m_pTailBucket = nullptr;
		other.m_iSize = 0;

		return *this;
	}

	THashMap& operator=(std::initializer_list<TKeyValuePair> ilist) {
		THashMap::Clear();
		ExpandIfNeeded(ilist.size());

		for (auto it = ilist.begin(); it != ilist.end(); ++it) {
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
			PushBackNewBucket(&m_pTable[uiBucket]);
		}

		m_pTable[uiBucket].EmplaceBack(TKeyValuePair{ static_cast<TKey>(Forward<Ky>(key)), static_cast<TValue>(Forward<Vy>(value))}, uiHash);
		++this->m_iSize;
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
			PushBackNewBucket(&m_pTable[uiBucket]);
		} 

		m_pTable[uiBucket].EmplaceBack(pair, uiHash);
		++this->m_iSize;
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
			PushBackNewBucket(&m_pTable[uiBucket]);
		}

		m_pTable[uiBucket].EmplaceBack(Move(pair), uiHash);
		++this->m_iSize;
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
			RemoveBucket(&bucket);
		}
	
		--this->m_iSize;
		return true;
	}

	void RemoveBucket(TBucket* bucket) {
		if (bucket == m_pHeadBucket) {
			m_pHeadBucket = m_pHeadBucket->Next;
			if (m_pHeadBucket == nullptr) m_pTailBucket = nullptr;
			else m_pHeadBucket->Previous = nullptr;
		} else if (bucket == m_pTailBucket) {
			m_pTailBucket = m_pTailBucket->Previous;
			if (m_pTailBucket == nullptr) m_pHeadBucket = nullptr;
			else m_pTailBucket->Next = nullptr;
		} else {
			ConnectBucket(bucket->Previous, bucket->Next);
		}
	}

	void Clear() noexcept override {
		if (this->m_iSize == 0) {
			return;
		}

		TBucket* pCur = m_pHeadBucket;
		while (pCur != nullptr) {
			TBucket* pTemp = pCur;
			pCur->Clear();
			pCur = pCur->Next;
		}

		this->m_iSize = 0;

		m_pHeadBucket = nullptr;
		m_pTailBucket = nullptr;
	}

	virtual bool Valid() const {
		return m_pTable != nullptr;
	}

	int BucketCount() {
		int iCount = 0;
		for (int i = 0; i < m_iCapacity; i++) {
			if (m_pTable[i].IsEmpty()) {
				continue;
			}
			++iCount;
		}
		return iCount;
	}


	// ==========================================
	// 동적할당 안하고 해쉬맵 순회할 수 있도록 기능 구현
	// ==========================================
	template <typename Consumer>
	void ForEach(Consumer&& consumer) {
		TBucket* pCurBucket = m_pHeadBucket;
		while (pCurBucket != nullptr) {
			for (int i = 0; i < pCurBucket->Size; i++) {
				TBucketNode& node = pCurBucket->GetAt(i);
				consumer(node.Pair);
			}
			pCurBucket = pCurBucket->Next;
		}
	}

	template <typename Consumer>
	void ForEachKey(Consumer&& consumer) {
		TBucket* pCurBucket = m_pHeadBucket;
		while (pCurBucket != nullptr) {
			for (int i = 0; i < pCurBucket->Size; i++) {
				TBucketNode& node = pCurBucket->GetAt(i);
				consumer(node.Pair.Key);
			}
			pCurBucket = pCurBucket->Next;
		}
	}

	template <typename Consumer>
	void ForEachValue(Consumer&& consumer) {
		TBucket* pCurBucket = m_pHeadBucket;
		while (pCurBucket != nullptr) {
			for (int i = 0; i < pCurBucket->Size; i++) {
				TBucketNode& node = pCurBucket->GetAt(i);
				consumer(node.Pair.Value);
			}
			pCurBucket = pCurBucket->Next;
		}
	}



	/*int Elements() {
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
	}*/

	SharedPtr<TIterator> Begin() const override {
		return MakeShared<THashMapIterator, TAllocator>(
			this->GetOwner(), 
			m_pHeadBucket,
			0
		);
	}

	SharedPtr<TIterator> End() const override {
		return MakeShared<THashMapIterator, TAllocator>(
			this->GetOwner(), 
			m_pTailBucket, 
			m_pTailBucket ? m_pTailBucket->Size - 1 : -1
		);
	}

	HashMapKeyCollection Keys() {
		return HashMapKeyCollection(this);
	}

	HashMapValueCollection Values() {
		return HashMapValueCollection(this);
	}

	void Expand(int capacity) {
		DebugAssertMsg(capacity > m_iCapacity, "이전 해쉬맵 크기보다 커야합니다.");

		int iAllocatedSize;
		TBucket* pNewTable = TAllocator::template Allocate<TBucket*>(sizeof(TBucket) * capacity, iAllocatedSize);
		Memory::PlacementNewArray(pNewTable, capacity);
		const int iPrevCapacity = m_iCapacity;

		m_iCapacity = capacity;
		m_pHeadBucket = nullptr;
		m_pTailBucket = nullptr;

		for (int i = 0; i < iPrevCapacity; i++) {
			if (m_pTable[i].Size == 0) {
				continue;
			}

			TBucket& prevBucket = m_pTable[i];

			// 기존 버킷을 순회하며 새로운 버킷에 데이터를 담아준다.
			for (int j = 0; j < prevBucket.Size; j++) {
				TBucketNode& bucketNode = prevBucket.GetAt(j);
				Int32U uiBucket = BucketIndex(bucketNode.Hash);

				if (pNewTable[uiBucket].IsEmpty()) {
					PushBackNewBucket(&pNewTable[uiBucket]);
				}

				pNewTable[uiBucket].PushBack(Move(bucketNode));
			}
		}

		JCORE_PLACEMENT_DELETE_ARRAY_SAFE(m_pTable, iPrevCapacity);
		JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(m_pTable, sizeof(TBucket) * iPrevCapacity);
		m_pTable = pNewTable;
	}

	ContainerType GetContainerType() override { return ContainerType::HashMap; }

	bool ExpandIfNeeded(int size) {
		if (size < m_iCapacity) {
			return false;
		}

		const int iCapacity = CalculateExpandCapacity(size);
		Expand(iCapacity);
		return true;
	}
protected:




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

	static void ConnectBucket(TBucket* lhs, TBucket* rhs) noexcept {
		if (lhs) lhs->Next = rhs;
		if (rhs) rhs->Previous = lhs;
	}

	/// <summary>
	/// bucket 바로 전에 다른 bucket을 삽입한다.
	/// </summary>
	void PushBackNewBucket(TBucket* bucket) {
		if (m_pHeadBucket == nullptr) {
			m_pHeadBucket = bucket;
			m_pTailBucket = bucket;
			return;
		}

		/*
			[삽입 전]
			 ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■
									       ↑
										  Tail

			[삽입 후]
			 ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■
			                                     ↑
									    	 newNode (tail)
		 */

		ConnectBucket(m_pTailBucket, bucket);
		m_pTailBucket = bucket;
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
	
	static constexpr Int32U	ms_iTableExpandingFactor = 4;	// 테이블 크기만큼 데이터가 들어가면 확장하는데 몇배나 확장할 지
	static constexpr Int32U	ms_iTableDefaultCapacity = 16;	// 테이블 초기 크기
protected:
	TBucket* m_pTable;
	TBucket* m_pHeadBucket;
	TBucket* m_pTailBucket;
	Int32U m_iCapacity;
public:
	struct HashMapKeyCollection : public TKeyCollection
	{
		using TEnumerator		= SharedPtr<Iterator<TKey, TAllocator>>;
		using TCollection		= Collection<TKey, TAllocator>;

		HashMapKeyCollection(THashMap* hashMap) : TKeyCollection(hashMap) {
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
			return MakeShared<HashMapKeyCollectionIterator, TAllocator>(
				m_pHashMap->GetOwner(), 
				m_pHashMap->m_pHeadBucket,
				0
			);
		}

		TEnumerator End() const override {
			return MakeShared<HashMapKeyCollectionIterator, TAllocator>(
				m_pHashMap->GetOwner(), 
				m_pHashMap->m_pTailBucket,
				m_pHashMap->m_pTailBucket ? m_pHashMap->m_pTailBucket->Size - 1 : -1
			);
		}

		ContainerType GetContainerType() override { return ContainerType::HashMapKeyCollection; }

		THashMap* m_pHashMap;
	};

	struct HashMapKeyCollectionIterator final : public TKeyCollectionIterator
	{
		using TListNode = ListNode<BucketNode<TKey, TValue>, TAllocator>;

		HashMapKeyCollectionIterator(VoidOwner& owner, TBucket* currentBucket, int currentBucketIndex)
			: m_HashMapIterator(owner, currentBucket, currentBucketIndex), TKeyCollectionIterator(owner, &m_HashMapIterator)
		{
		}
		virtual ~HashMapKeyCollectionIterator() noexcept = default;

		THashMapIterator m_HashMapIterator;
	};

	struct HashMapValueCollection final : public TValueCollection
	{
		using TEnumerator		= SharedPtr<Iterator<TValue, TAllocator>>;
		using TCollection		= Collection<TValue, TAllocator>;

		HashMapValueCollection(THashMap* hashMap) : TMapCollection::ValueCollection(hashMap) {
			m_pHashMap = hashMap;
		}

		virtual ~HashMapValueCollection() noexcept override = default;

		HashMapValueCollection& operator=(const HashMapValueCollection& other) {
			this->m_pHashMap = other.m_pHashMap;
			this->m_pMap = other.m_pHashMap;
			return *this;
		}

		TEnumerator Begin() const override {
			return MakeShared<HashMapValueCollectionIterator, TAllocator>(
				m_pHashMap->GetOwner(),
				m_pHashMap->m_pHeadBucket,
				0
			);
		}

		TEnumerator End() const override {
			return MakeShared<HashMapValueCollectionIterator, TAllocator>(
				m_pHashMap->GetOwner(),
				m_pHashMap->m_pTailBucket,
				m_pHashMap->m_pTailBucket ? m_pHashMap->m_pTailBucket->Size - 1 : -1
			);
		}

		ContainerType GetContainerType() override { return ContainerType::HashMapValueCollection; }

		THashMap* m_pHashMap;
	};

	struct HashMapValueCollectionIterator final : public TValueCollectionIterator
	{
		using TListNode = ListNode<BucketNode<TKey, TValue>, TAllocator>;

		HashMapValueCollectionIterator(VoidOwner& owner, TBucket* currentBucket, int currentBucketIndex)
			: m_HashMapIterator(owner, currentBucket, currentBucketIndex), TValueCollectionIterator(owner, &m_HashMapIterator)
		{
		}

		virtual ~HashMapValueCollectionIterator() noexcept override = default;

		THashMapIterator m_HashMapIterator;
	};


	friend class THashMapIterator;

}; // class HashMap<TKey, TValue>

NS_JC_END

