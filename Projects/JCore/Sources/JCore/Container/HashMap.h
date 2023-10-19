/*
 *	작성자 : 윤정도
 */

#pragma once

#include <JCore/Container/HashTable.h>
#include <JCore/Container/MapCollection.h>
#include <JCore/Container/HashMapIterator.h>

NS_JC_BEGIN

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
	using THashTable			    = HashTable<TKey, TValue, TAllocator>;
	using TKeyValuePair				= Pair<TKey, TValue>;
	using TMapCollection			= MapCollection<TKey, TValue, TAllocator>;
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
	HashMap(int capacity = THashTable::ms_iTableDefaultCapacity)
		: TMapCollection()
		, Table(capacity)
	{}

	HashMap(const THashMap& other) : THashMap(other.Table.m_iCapacity) {
		operator=(other);
	}

	HashMap(THashMap&& other) noexcept {
		operator=(Move(other));
	}

	// 이니셜라이저로 초기화 하는 경우 보통 더 확장안시킬 확률이 크므로.. 맞춤형으로 가자.
	HashMap(std::initializer_list<TKeyValuePair> ilist) : THashMap(ilist.size() + 1) {
		operator=(ilist);
	}

	~HashMap() noexcept override {}
public:


	THashMap& operator=(const THashMap& other) {
		Table.operator=(other.Table);
		return *this;
	}

	THashMap& operator=(THashMap&& other) noexcept {
		Table.operator=(Move(other.Table));
		return *this;
	}

	THashMap& operator=(std::initializer_list<TKeyValuePair> ilist) {
		Table.operator=(ilist);
		return *this;
	}

	TValue& operator[](const TKey& key) {
		return Table.Get(key);
	}

	template <typename Ky, typename Vy>
	bool Insert(Ky&& key, Vy&& value) {
		return Table.Insert(Forward<Ky>(key), Forward<Vy>(value));
	}

	bool Insert(const TKeyValuePair& pair) override {
		return Table.Insert(pair.Key, pair.Value);
	}

	bool Insert(TKeyValuePair&& pair) override {
		return Table.Insert(Move(pair.Key), Move(pair.Value));
	}

	template <typename = DefaultEnableIf_t<IsStringType_v<TKey>>>
	bool Exist(const char* key) const {
		return Table.Exist(key);
	}

	bool Exist(const TKey& key) const override {
		return Table.Exist(key);
	}

	template <typename = DefaultEnableIf_t<IsStringType_v<TKey>>>
	TValue* Find(const char* key) const {
		return Table.Find(key);
	}

	TValue* Find(const TKey& key) const {
		return Table.Find(key);
	}

	TValue& Get(const TKey& key) const override {
		return Table.Get(key);
	}

	template <typename = DefaultEnableIf_t<IsStringType_v<TKey>>>
	bool Remove(const char* key) {
		return Table.Remove(key);
	}

	bool Remove(const TKey& key) override {
		return Table.Remove(key);
	}

	void Clear() noexcept override {
		Table.Clear();
	}

	typename THashTable::TBucket* Bucket(int index) const {
		return Table.Bucket();
	}

	int BucketCount() {
		return Table.BucketCount();
	}

	int Size() const override {
		return Table.Size();
	}

	bool IsEmpty() const override {
		return Table.Size() == 0;
	}

	bool ExpandIfNeeded(int size) {
		return Table.ExpandIfNeeded(size);
	}

	// ==========================================
	// 동적할당 안하고 해쉬맵 순회할 수 있도록 기능 구현
	// ==========================================
	template <typename Consumer>
	void ForEach(Consumer&& consumer) {
		Table.ForEach(Forward<Consumer>(consumer));
	}

	template <typename Consumer>
	void ForEachKey(Consumer&& consumer) {
		Table.ForEachKey(Forward<Consumer>(consumer));
	}

	template <typename Consumer>
	void ForEachValue(Consumer&& consumer) {
		Table.ForEachValue(Forward<Consumer>(consumer));
	}

	// Value들만 순회해서 삭제하는 작업
	// 자주 사용해서 그냥 라이브러리에 박음
	void ForEachValueDelete() {
		Table.ForEachValueDelete();
	}

	void ForEachValueRelease() {
		Table.ForEachValueRelease();
	}

	SharedPtr<TIterator> Begin() const override {
		return MakeShared<THashMapIterator, TAllocator>(
			this->GetOwner(), 
			Table.m_pHeadBucket,
			0
		);
	}

	SharedPtr<TIterator> End() const override {
		return MakeShared<THashMapIterator, TAllocator>(
			this->GetOwner(), 
			Table.m_pTailBucket,
			Table.m_pTailBucket ? Table.m_pTailBucket->Size - 1 : -1
		);
	}

	HashMapKeyCollection Keys() {
		return HashMapKeyCollection(this);
	}

	HashMapValueCollection Values() {
		return HashMapValueCollection(this);
	}

	ContainerType GetContainerType() override { return ContainerType::HashMap; }
protected:
	THashTable Table;
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

		~HashMapKeyCollection() noexcept override = default;

		int Size() const override {
			return TKeyCollection::Size();
		}

		bool IsEmpty() const override {
			return TKeyCollection::IsEmpty();
		}

		TEnumerator Begin() const override {
			return MakeShared<HashMapKeyCollectionIterator, TAllocator>(
				m_pHashMap->GetOwner(), 
				m_pHashMap->Table.m_pHeadBucket,
				0
			);
		}

		TEnumerator End() const override {
			return MakeShared<HashMapKeyCollectionIterator, TAllocator>(
				m_pHashMap->GetOwner(), 
				m_pHashMap->Table.m_pTailBucket,
				m_pHashMap->Table.m_pTailBucket ? m_pHashMap->Table.m_pTailBucket->Size - 1 : -1
			);
		}

		ContainerType GetContainerType() override { return ContainerType::HashMapKeyCollection; }

		THashMap* m_pHashMap;
	};

	struct HashMapKeyCollectionIterator final : public TKeyCollectionIterator
	{
		HashMapKeyCollectionIterator(VoidOwner& owner, typename THashTable::TBucket* currentBucket, int currentBucketIndex)
			: TKeyCollectionIterator(owner, &m_HashMapIterator), m_HashMapIterator(owner, currentBucket, currentBucketIndex)
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
				m_pHashMap->Table.m_pHeadBucket,
				0
			);
		}

		TEnumerator End() const override {
			return MakeShared<HashMapValueCollectionIterator, TAllocator>(
				m_pHashMap->GetOwner(),
				m_pHashMap->Table.m_pTailBucket,
				m_pHashMap->Table.m_pTailBucket ? m_pHashMap->Table.m_pTailBucket->Size - 1 : -1
			);
		}

		ContainerType GetContainerType() override { return ContainerType::HashMapValueCollection; }

		THashMap* m_pHashMap;
	};

	struct HashMapValueCollectionIterator final : public TValueCollectionIterator
	{
		HashMapValueCollectionIterator(VoidOwner& owner, typename THashTable::TBucket* currentBucket, int currentBucketIndex)
			: TValueCollectionIterator(owner, &m_HashMapIterator), m_HashMapIterator(owner, currentBucket, currentBucketIndex)
		{}

		~HashMapValueCollectionIterator() noexcept override = default;

		THashMapIterator m_HashMapIterator;
	};


	friend class THashMapIterator;

}; // class HashMap<TKey, TValue>

NS_JC_END

