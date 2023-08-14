/*
 * 작성자: 윤정도
 * 생성일: 8/14/2023 9:33:27 AM
 * =====================
 * 해쉬맵만 쓰다가 해쉬셋이 필요해져서 하나의 클래스로 표현하기 위해 HashTable을 정의함
 */


#pragma once


#pragma once

#include <JCore/Hasher.h>
#include <JCore/Memory.h>
#include <JCore/Container/Pair.h>

NS_JC_BEGIN

template <typename T>
struct BucketNode
{
	using TBucketNode = BucketNode<T>;

	bool operator==(const TBucketNode& other) {
		return Data == other.Data && Hash == other.Hash;
	}

	T Data;
	Int32U Hash{};	// 처음에 한번 계산해놓으면 성능이 좀더 개선될 듯?
};

 /* ==============================================================
  *
  * 12월 25일 일요일 크리스마스 기념! 버킷을 공허의 다이나믹 배열로 변경
  * 공허의 다이나믹 배열?: 데이터를 넣을때 초기화를 진행한다! 데이터를 넣기전까진 無의 상태
  * 보통 1개의 데이터를 넣게 되므로 초기 용량은 1로 설정하고 이후 +2씩 커지도록 하였다.
  * 근데 3개까지 찰 확률은 거의 없다.
  *
  * ============================================================== */
template <typename...>
struct Bucket;

#pragma region Bucket<TKey, TAllocator>

// 해쉬셋을 위한 버킷
template <typename TKey, typename TAllocator>
struct Bucket<TKey, TAllocator>
{
	using TBucketNode = BucketNode<TKey>;

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
		DynamicArray = TAllocator::template AllocateDynamic<TBucketNode*>(sizeof(TBucketNode) * Capacity, iAllocated);
		// Memory::Set(DynamicArray, sizeof(TBucketNode) * Capacity, 0);
	}

	void Expand(int newCapacity) {
		int iAllocated;
		TBucketNode* pNewDynamicArray = TAllocator::template AllocateDynamic<TBucketNode*>(sizeof(TBucketNode) * newCapacity, iAllocated);

		if constexpr (!IsFundamentalType_v<TKey> && !IsPointerType_v<TKey>) {
			if constexpr (IsStringType_v<TKey>)
				Memory::PlacementNewArray(pNewDynamicArray, newCapacity, TBucketNode{ { 0 }, 0 });	// 문자열은 동적할당 안된 상태로 생성해주자. String(0)는 동적할당안함
			else
				DebugAssert(false); // String도 아니고 int같은 기본 타입도 아닌 새로운 키타입을 추가하고자 한다면 성능향상을 위해 여기서 직접 수정 ㄱ, 그냥 Memory::PlacementNewArray(pNewDynamicArray, newCapacity)를 수행해도 동작하는데 문제없긴함
		}

		for (int i = 0; i < Size; i++) {
			pNewDynamicArray[i] = Move(DynamicArray[i]);	// 2023/02/23 Move 필수.. Value가 포인터라지만 Key가 String 같은 타입일 수가 있다. 만약 Move를 안하고 Copy를 한다면 DynamicArray의 메모리를 해제하기전에 수동으로 소멸자 호출을 해줘야함.
		}

		TAllocator::DeallocateDynamic(DynamicArray, sizeof(TBucketNode) * Capacity);
		DynamicArray = pNewDynamicArray;
		Capacity = newCapacity;
	}

	void Clear() {
		JCORE_PLACEMENT_DELETE_ARRAY_SAFE(DynamicArray, Size);
		Size = 0;
	}

	bool Exist(const TKey& key) {
		for (int i = 0; i < Size; i++) {
			if (DynamicArray[i].Data == key) {
				return true;
			}
		}

		return false;
	}

	TKey* Find(const TKey& key) {
		for (int i = 0; i < Size; i++) {
			if (DynamicArray[i].Data == key) {
				return AddressOf(DynamicArray[i].Data);
			}
		}

		return nullptr;
	}

	bool Remove(const TKey& key) {
		int iFind = -1;
		for (int i = 0; i < Size; i++) {
			if (DynamicArray[i].Data == key) {
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

#pragma endregion

#pragma region Bucket<TKey, TValue, TAllocator>
template <typename TKey, typename TValue, typename TAllocator>
struct Bucket<TKey, TValue, TAllocator>
{
	using TBucketNode = BucketNode<Pair<TKey, TValue>>;

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
		DynamicArray = TAllocator::template AllocateDynamic<TBucketNode*>(sizeof(TBucketNode) * Capacity, iAllocated);
		// Memory::Set(DynamicArray, sizeof(TBucketNode) * Capacity, 0);
	}

	void Expand(int newCapacity) {
		int iAllocated;
		TBucketNode* pNewDynamicArray = TAllocator::template AllocateDynamic<TBucketNode*>(sizeof(TBucketNode) * newCapacity, iAllocated);

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

		TAllocator::DeallocateDynamic(DynamicArray, sizeof(TBucketNode) * Capacity);
		DynamicArray = pNewDynamicArray;
		Capacity = newCapacity;
	}

	void Clear() {
		JCORE_PLACEMENT_DELETE_ARRAY_SAFE(DynamicArray, Size);
		Size = 0;
	}

	bool Exist(const TKey& key) {
		for (int i = 0; i < Size; i++) {
			if (DynamicArray[i].Data.Key == key) {
				return true;
			}
		}

		return false;
	}

	TValue* Find(const TKey& key) {
		for (int i = 0; i < Size; i++) {
			if (DynamicArray[i].Data.Key == key) {
				return AddressOf(DynamicArray[i].Data.Value);
			}
		}

		return nullptr;
	}

	bool Remove(const TKey& key) {
		int iFind = -1;
		for (int i = 0; i < Size; i++) {
			if (DynamicArray[i].Data.Key == key) {
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
#pragma endregion

template <typename...>
class HashTable;

#pragma region HashTable<TKey>
template <typename TKey, typename TAllocator>
class HashTable<TKey, TAllocator>
{
public:
	using THashTable = HashTable<TKey, TAllocator>;
	using THasher = Hasher<TKey>;
	using TBucket = Bucket<TKey, TAllocator>;
	using TBucketNode = BucketNode<TKey>;
public:
	HashTable(int capacity = ms_iTableDefaultCapacity)
		: m_pTable(nullptr)
		, m_pHeadBucket(nullptr)
		, m_pTailBucket(nullptr)
		, m_iCapacity(capacity)
	{
		if (m_iCapacity > 0) {
			int iAllocatedSize;
			m_pTable = TAllocator::template AllocateDynamic<TBucket*>(sizeof(TBucket) * capacity, iAllocatedSize);
			Memory::PlacementNewArray(m_pTable, capacity);
		}
	}

	HashTable(const THashTable& other) : THashTable(other.m_iCapacity) {
		operator=(other);
	}

	HashTable(THashTable&& other) noexcept
		: m_pTable(nullptr)
		, m_pHeadBucket(nullptr)
		, m_pTailBucket(nullptr)
	{
		operator=(Move(other));
	}

	// 이니셜라이저로 초기화 하는 경우 보통 더 확장안시킬 확률이 크므로.. 맞춤형으로 가자.
	HashTable(std::initializer_list<TKey> ilist) : THashTable(ilist.size() + 1) {
		operator=(ilist);
	}

	~HashTable() noexcept {
		Clear();

		JCORE_PLACEMENT_DELETE_ARRAY_SAFE(m_pTable, m_iCapacity);
		JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(m_pTable, m_iCapacity * sizeof(TBucket));
	}
public:
	THashTable& operator=(const THashTable& other) {
		Clear();
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

	THashTable& operator=(THashTable&& other) noexcept {
		Clear();
		JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(m_pTable, sizeof(TBucket) * m_iCapacity);

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

	THashTable& operator=(std::initializer_list<TKey> ilist) {
		Clear();
		ExpandIfNeeded(ilist.size());

		for (auto it = ilist.begin(); it != ilist.end(); ++it) {
			Insert((*it));
		}

		return *this;
	}

	template <typename Ky>
	bool Insert(Ky&& key) {
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

		m_pTable[uiBucket].EmplaceBack(static_cast<TKey>(Forward<Ky>(key), uiHash));
		++this->m_iSize;
		return true;
	}

	bool Exist(const TKey& key) const {
		if (m_pTable == nullptr) return false;
		return m_pTable[HashBucket(key)].ExistByKey(key);
	}

	bool Remove(const TKey& key) {
		TBucket& bucket = m_pTable[HashBucket(key)];

		if (!bucket.Remove(key)) {
			return false;
		}

		// 버킷이 비었으면 연결을 끊어준다.
		if (bucket.IsEmpty()) {
			DisjointBucket(&bucket);
		}

		--this->m_iSize;
		return true;
	}

	void DisjointBucket(TBucket* bucket) {
		if (bucket == m_pHeadBucket) {
			m_pHeadBucket = m_pHeadBucket->Next;
			if (m_pHeadBucket == nullptr) m_pTailBucket = nullptr;
			else m_pHeadBucket->Previous = nullptr;
		}
		else if (bucket == m_pTailBucket) {
			m_pTailBucket = m_pTailBucket->Previous;
			if (m_pTailBucket == nullptr) m_pHeadBucket = nullptr;
			else m_pTailBucket->Next = nullptr;
		}
		else {
			ConnectBucket(bucket->Previous, bucket->Next);
		}
	}

	void Clear() noexcept {
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

	bool Valid() const {
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
				consumer(node.Data);
			}
			pCurBucket = pCurBucket->Next;
		}
	}

	void ForEachDelete() {
		if constexpr (!IsPointerType_v<TKey>) {
			DebugAssert(false);
			return;
		}

		TBucket* pCurBucket = m_pHeadBucket;
		while (pCurBucket != nullptr) {
			for (int i = 0; i < pCurBucket->Size; i++) {
				TBucketNode& node = pCurBucket->GetAt(i);
				delete node.Data;
			}
			pCurBucket = pCurBucket->Next;
		}

		Clear();
	}

	void Expand(int capacity) {

		// 0 용량 해쉬맵 기능 추가때문에
		if (capacity == 0)
			capacity = 4;

		DebugAssertMsg(capacity > m_iCapacity, "이전 해쉬맵 크기보다 커야합니다.");

		int iAllocatedSize;
		TBucket* pNewTable = TAllocator::template AllocateDynamic<TBucket*>(sizeof(TBucket) * capacity, iAllocatedSize);
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

	bool ExpandIfNeeded(int size) {
		if (size <= 0 || size < m_iCapacity) {
			return false;
		}

		const int iCapacity = CalculateExpandCapacity(size);
		Expand(iCapacity);
		return true;
	}

	int Size() const {
		return m_iSize;
	}

	int Capacity() const {
		return m_iCapacity;
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
	Int32 m_iCapacity;
	Int32 m_iSize;
}; // class HashTable<TKey, TAllocator>

#pragma endregion

#pragma region HashTable<TKey, TValue>
template <typename TKey, typename TValue, typename TAllocator>
class HashTable<TKey, TValue, TAllocator>
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
	using THashTable		= HashTable<TKey, TValue, TAllocator>;
	using THasher			= Hasher<TKey>;
	using TBucket			= Bucket<TKey, TValue, TAllocator>;
	using TBucketNode		= BucketNode<Pair<TKey, TValue>>;
	using TKeyValuePair		= Pair<TKey, TValue>;

	HashTable(int capacity = ms_iTableDefaultCapacity)
		: m_pTable(nullptr)
		, m_pHeadBucket(nullptr)
		, m_pTailBucket(nullptr)
		, m_iCapacity(capacity)
		, m_iSize(0)
	{
		if (m_iCapacity > 0) {
			int iAllocatedSize;
			m_pTable = TAllocator::template AllocateDynamic<TBucket*>(sizeof(TBucket) * capacity, iAllocatedSize);
			Memory::PlacementNewArray(m_pTable, capacity);
		} 
	}

	HashTable(const THashTable& other) : THashTable(other.m_iCapacity) {
		operator=(other);
	}

	HashTable(THashTable&& other) noexcept
		: m_pTable(nullptr)
		, m_pHeadBucket(nullptr)
		, m_pTailBucket(nullptr)
	{
		operator=(Move(other));
	}

	// 이니셜라이저로 초기화 하는 경우 보통 더 확장안시킬 확률이 크므로.. 맞춤형으로 가자.
	HashTable(std::initializer_list<TKeyValuePair> ilist) : THashTable(ilist.size() + 1) {
		operator=(ilist);
	}

	~HashTable() noexcept {
		Clear();

		JCORE_PLACEMENT_DELETE_ARRAY_SAFE(m_pTable, m_iCapacity);
		JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(m_pTable, m_iCapacity * sizeof(TBucket));
	}
public:


	THashTable& operator=(const THashTable& other) {
		Clear();
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

	THashTable& operator=(THashTable&& other) noexcept {
		Clear();
		JCORE_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(m_pTable, sizeof(TBucket) * m_iCapacity);

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

	THashTable& operator=(std::initializer_list<TKeyValuePair> ilist) {
		Clear();
		ExpandIfNeeded(ilist.size());

		for (auto it = ilist.begin(); it != ilist.end(); ++it) {
			Insert(Move(it->Key), Move(it->Value));
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

	bool Exist(const TKey& key) const {
		if (m_pTable == nullptr) return false;
		return m_pTable[HashBucket(key)].Exist(key);
	}

	TValue* Find(const TKey& key) const {
		if (m_pTable == nullptr) return nullptr;

		TValue* pVal = m_pTable[HashBucket(key)].Find(key);

		if (pVal == nullptr) {
			return nullptr;
		}

		return pVal;
	}

	TValue& Get(const TKey& key) const {
		if (m_pTable == nullptr) 
			throw InvalidOperationException("초기화 되지 않은 해쉬맵입니다.");;

		TValue* pVal = m_pTable[HashBucket(key)].Find(key);

		if (pVal == nullptr) {
			throw InvalidArgumentException("해당 키값에 대응하는 값이 존재하지 않습니다.");
		}

		return *pVal;
	}

	bool Remove(const TKey& key) {
		TBucket& bucket = m_pTable[HashBucket(key)];

		if (!bucket.Remove(key)) {
			return false;
		}

		// 버킷이 비었으면 연결을 끊어준다.
		if (bucket.IsEmpty()) {
			DisjointBucket(&bucket);
		}
	
		--this->m_iSize;
		return true;
	}

	void DisjointBucket(TBucket* bucket) {
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

	void Clear() noexcept {
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

	bool Valid() const {
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
				consumer(node.Data);
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
				consumer(node.Data.Key);
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
				consumer(node.Data.Value);
			}
			pCurBucket = pCurBucket->Next;
		}
	}

	// Value들만 순회해서 삭제하는 작업
	// 자주 사용해서 그냥 라이브러리에 박음
	void ForEachValueDelete() {
		if constexpr (!IsPointerType_v<TValue>) {
			DebugAssert(false);
			return;
		}

		TBucket* pCurBucket = m_pHeadBucket;
		while (pCurBucket != nullptr) {
			for (int i = 0; i < pCurBucket->Size; i++) {
				TBucketNode& node = pCurBucket->GetAt(i);
				delete node.Data.Value;
			}
			pCurBucket = pCurBucket->Next;
		}
	}

	void Expand(int capacity) {

		// 0 용량 해쉬맵 기능 추가때문에
		if (capacity == 0)
			capacity = 4;

		DebugAssertMsg(capacity > m_iCapacity, "이전 해쉬맵 크기보다 커야합니다.");

		int iAllocatedSize;
		TBucket* pNewTable = TAllocator::template AllocateDynamic<TBucket*>(sizeof(TBucket) * capacity, iAllocatedSize);
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

	bool ExpandIfNeeded(int size) {
		if (size <= 0 || size < m_iCapacity) {
			return false;
		}

		const int iCapacity = CalculateExpandCapacity(size);
		Expand(iCapacity);
		return true;
	}

	int Size() const {
		return m_iSize;
	}

	int Capacity() const {
		return m_iCapacity;
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
	Int32 m_iCapacity;
	Int32 m_iSize;

	template <typename, typename, typename> friend class HashMap;
}; // class HashTable<TKey, TValue, TAllocator>

#pragma endregion

NS_JC_END