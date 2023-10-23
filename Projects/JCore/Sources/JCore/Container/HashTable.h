/*
 * 작성자: 윤정도
 * 생성일: 8/14/2023 9:33:27 AM
 * =====================
 * 해쉬맵만 쓰다가 해쉬셋이 필요해져서 하나의 클래스로 표현하기 위해 HashTable을 정의함
 */


#pragma once

#include <JCore/Container/HashTable_Bucket.h>

NS_JC_BEGIN


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
				TBucket& bucket = m_pTable[uiBucket];

				if (bucket.IsEmpty()) {
					PushBackNewBucket(&bucket);
				}

				bucket.PushBack(node);
				
			}
			pOtherBucketCur = pOtherBucketCur->Next;
		}

		this->m_iSize = other.m_iSize;
		return *this;
	}

	THashTable& operator=(THashTable&& other) noexcept {
		Clear();

		JCORE_PLACEMENT_DELETE_ARRAY_SAFE(m_pTable, m_iCapacity);
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
		TBucket& bucket = m_pTable[uiBucket];

		if (bucket.IsEmpty()) {
			PushBackNewBucket(&bucket);
		}

		bucket.EmplaceBack(Forward<Ky>(key), uiHash);
		++this->m_iSize;
		return true;
	}

	template <typename Ky>
	bool Exist(const Ky& key) const {
		if (m_pTable == nullptr) return false;
		return m_pTable[HashBucket(key)].Exist(key);
	}

	template <typename Ky>
	bool Remove(const Ky& key) {
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
		bucket->Previous = nullptr;
		bucket->Next = nullptr;
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


	TBucket* Bucket(int index) const {
		if (index < 0 || index >= m_iCapacity) {
			DebugAssert(false);
			return nullptr;
		}

		return m_pTable[index];
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

	template <typename Ky>
	Int32U Hash(const Ky& key) const {
		return THasher()(key);
	}

	template <typename Ky>
	Int32U HashBucket(const Ky& key) const {
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

	template <typename, typename> friend class HashSet;
	template <typename, typename> friend class HashSetIterator;
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
				TBucket& bucket = m_pTable[uiBucket];
				
				if (bucket.IsEmpty()) {
					PushBackNewBucket(&bucket);
				}

				bucket.PushBack(node);

			}
			pOtherBucketCur = pOtherBucketCur->Next;
		}

		this->m_iSize = other.m_iSize;
		return *this;
	}

	THashTable& operator=(THashTable&& other) noexcept {
		Clear();

		JCORE_PLACEMENT_DELETE_ARRAY_SAFE(m_pTable, m_iCapacity);
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
		TBucket& bucket = m_pTable[uiBucket];

		if (bucket.IsEmpty()) {
			PushBackNewBucket(&bucket);
		}

		bucket.EmplaceBack(TKeyValuePair{ static_cast<TKey>(Forward<Ky>(key)), static_cast<TValue>(Forward<Vy>(value))}, uiHash);
		++this->m_iSize;
		return true;
	}

	template <typename Ky>
	bool Exist(const Ky& key) const {
		if (m_pTable == nullptr) return false;
		return m_pTable[HashBucket(key)].Exist(key);
	}

	template <typename Ky>
	TValue* Find(const Ky& key) const {
		if (m_pTable == nullptr) return nullptr;

		TValue* pVal = m_pTable[HashBucket(key)].Find(key);

		if (pVal == nullptr) {
			return nullptr;
		}

		return pVal;
	}

	template <typename Ky>
	TValue& Get(const Ky& key) const {
		if (m_pTable == nullptr) 
			throw InvalidOperationException("초기화 되지 않은 해쉬맵입니다.");;

		TValue* pVal = m_pTable[HashBucket(key)].Find(key);

		if (pVal == nullptr) {
			throw InvalidArgumentException("해당 키값에 대응하는 값이 존재하지 않습니다.");
		}

		return *pVal;
	}

	template <typename Ky>
	bool Remove(const Ky& key) {
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

		// 연결리스트는 발견된 노드를 삭제하기 때문에 Next, Previous를 null로 설정할 필요가 없었는데
		// 해쉬테이블은 노드를 삭제한게 아니기 땜에 Next, Previous를 null로 설정해놔야한다. ㅠㅠ
		// 작성된 코드에는 문제가 없었고.. 추가를 해줬어야했다 ㅠㅠ 논리적 오류가 있을 줄 알앗는데.. 아니었다.
		// 1시간 30분동안이나 디버깅 해서 겨우 찾음 ㅠㅠ;
		bucket->Next = nullptr;
		bucket->Previous = nullptr;
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

	TBucket* Bucket(int index) const {
		if (index < 0 || index >= m_iCapacity) {
			DebugAssert(false);
			return nullptr;
		}

		return m_pTable[index];
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

	void ForEachValueRelease() {
		if constexpr (!IsPointerType_v<TValue>) {
			DebugAssert(false);
			return;
		}

		TBucket* pCurBucket = m_pHeadBucket;
		while (pCurBucket != nullptr) {
			for (int i = 0; i < pCurBucket->Size; i++) {
				TBucketNode& node = pCurBucket->GetAt(i);
				node.Data.Value->Release();
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

	template <typename Ky>
	Int32U Hash(const Ky& key) const {
		return THasher()(key);
	}

	template <typename Ky>
	Int32U HashBucket(const Ky& key) const {
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
	template <typename, typename, typename> friend class HashMapIterator;

	template <typename, typename> friend class Properties;
	template <typename, typename> friend class PropertiesIterator;
}; // class HashTable<TKey, TValue, TAllocator>

#pragma endregion

NS_JC_END