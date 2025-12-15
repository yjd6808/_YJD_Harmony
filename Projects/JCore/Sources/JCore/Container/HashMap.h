/*
 *	작성자 : 윤정도
 */

#pragma once

#include <JCore/Container/HashTable.h>
#include <JCore/Container/MapCollection.h>
#include <JCore/Container/HashMapIterator.h>

NS_JC_BEGIN
template <typename TKey, typename TValue, typename TAllocator = CDefaultAllocator>
class HashMap : public MapCollection<TKey, TValue, TAllocator>
{
	using TKeyValuePair = Pair<TKey, TValue>;
	using TMapCollection = MapCollection<TKey, TValue, TAllocator>;
	using TIterator = Iterator<TKeyValuePair, TAllocator>;
	using THashMap = HashMap<TKey, TValue, TAllocator>;
	using THashMapIterator = HashMapIterator<TKey, TValue, TAllocator>;
	using TKeyCollection = typename TMapCollection::KeyCollection;
	using TValueCollection = typename TMapCollection::ValueCollection;
	using TKeyCollectionIterator = typename TMapCollection::KeyCollectionIterator;
	using TValueCollectionIterator = typename TMapCollection::ValueCollectionIterator;

public:
	// 내부 구조체 전방 선언 (inner struct forward declaration)
	struct HashMapKeyCollection;
	struct HashMapKeyCollectionIterator;
	struct HashMapValueCollection;
	struct HashMapValueCollectionIterator;

public:
	HashMap(int _capacity = HashTable<TKey, TValue, TAllocator>::TABLE_DEFAULT_CAPACITY)
	: TMapCollection()
	, hashTable_(_capacity)
	{
	}

	HashMap(const THashMap& _other)
	: THashMap(_other.hashTable_.capacity_)
	{
		operator=(_other);
	}

	HashMap(THashMap&& _other) noexcept
	{
		operator=(Move(_other));
	}

	// 이니셜라이저로 초기화 하는 경우 보통 더 확장안시킬 확률이 크므로.. 맞춤형으로 가자.
	HashMap(std::initializer_list<TKeyValuePair> _ilist)
	: THashMap(static_cast<int>(_ilist.size()) + 1)
	{
		operator=(_ilist);
	}

	~HashMap() noexcept override = default;

public:
	THashMap& operator=(const THashMap& _other)
	{
		hashTable_.operator=(_other.hashTable_);
		return *this;
	}

	THashMap& operator=(THashMap&& _other) noexcept
	{
		hashTable_.operator=(Move(_other.hashTable_));
		return *this;
	}

	THashMap& operator=(std::initializer_list<TKeyValuePair> _ilist)
	{
		hashTable_.operator=(_ilist);
		return *this;
	}

	TValue& operator[](const TKey& _key)
	{
		return hashTable_.Get(_key);
	}

	template <typename Ky, typename Vy>
	bool Insert(Ky&& _key, Vy&& _value)
	{
		return hashTable_.Insert(Forward<Ky>(_key), Forward<Vy>(_value));
	}

	bool Insert(const TKeyValuePair& _pair) override
	{
		return hashTable_.Insert(_pair.key_, _pair.value_);
	}

	bool Insert(TKeyValuePair&& _pair) override
	{
		return hashTable_.Insert(Move(_pair.key_), Move(_pair.value_));
	}

	template <typename = DefaultEnableIf_t<IsStringType_v<TKey>>>
	bool Exist(const char* _pKey) const
	{
		return hashTable_.Exist(_pKey);
	}

	bool Exist(const TKey& _key) const override
	{
		return hashTable_.Exist(_key);
	}

	template <typename = DefaultEnableIf_t<IsStringType_v<TKey>>>
	TValue* Find(const char* _pKey) const
	{
		return hashTable_.Find(_pKey);
	}

	TValue* Find(const TKey& _key) const
	{
		return hashTable_.Find(_key);
	}

	TValue& Get(const TKey& _key) const override
	{
		return hashTable_.Get(_key);
	}

	template <typename = DefaultEnableIf_t<IsStringType_v<TKey>>>
	bool Remove(const char* _pKey)
	{
		return hashTable_.Remove(_pKey);
	}

	bool Remove(const TKey& _key) override
	{
		return hashTable_.Remove(_key);
	}

	void Clear() noexcept override
	{
		hashTable_.Clear();
	}

	typename HashTable<TKey, TValue, TAllocator>::TBucket* Bucket() const
	{
		return hashTable_.Bucket();
	}

	int BucketCount()
	{
		return hashTable_.BucketCount();
	}

	int Size() const override
	{
		return hashTable_.Size();
	}

	bool IsEmpty() const override
	{
		return hashTable_.Size() == 0;
	}

	bool ExpandIfNeeded(int _size)
	{
		return hashTable_.ExpandIfNeeded(_size);
	}

	// ==========================================
	// 동적할당 안하고 해쉬맵 순회할 수 있도록 기능 구현
	// ==========================================
	template <typename Consumer>
	void ForEach(Consumer&& _consumer)
	{
		hashTable_.ForEach(Forward<Consumer>(_consumer));
	}

	template <typename Consumer>
	void ForEachKey(Consumer&& _consumer)
	{
		hashTable_.ForEachKey(Forward<Consumer>(_consumer));
	}

	template <typename Consumer>
	void ForEachValue(Consumer&& _consumer)
	{
		hashTable_.ForEachValue(Forward<Consumer>(_consumer));
	}

	// Value들만 순회해서 삭제하는 작업
	// 자주 사용해서 그냥 라이브러리에 박음
	void ForEachValueDelete()
	{
		hashTable_.ForEachValueDelete();
	}

	void ForEachValueRelease()
	{
		hashTable_.ForEachValueRelease();
	}

	SharedPtr<TIterator> Begin() const override
	{
		return MakeShared<THashMapIterator, TAllocator>(
			this->GetOwner(),
			hashTable_.pHeadBucket_,
			0
		);
	}

	SharedPtr<TIterator> End() const override
	{
		return MakeShared<THashMapIterator, TAllocator>(
			this->GetOwner(),
			hashTable_.pTailBucket_,
			hashTable_.pTailBucket_ ? hashTable_.pTailBucket_->size_ - 1 : -1
		);
	}

	HashMapKeyCollection Keys()
	{
		return HashMapKeyCollection(this);
	}

	HashMapValueCollection Values()
	{
		return HashMapValueCollection(this);
	}

	ContainerType GetContainerType() override
	{
		return ContainerType::HashMap;
	}

protected:
	HashTable<TKey, TValue, TAllocator> hashTable_;

public:
	struct HashMapKeyCollection : public TKeyCollection
	{
		using TEnumerator = SharedPtr<Iterator<TKey, TAllocator>>;
		using TCollection = Collection<TKey, TAllocator>;

		HashMapKeyCollection(THashMap* _pHashMap)
		: TKeyCollection(_pHashMap)
		, pHashMap_(_pHashMap)
		{
		}

		HashMapKeyCollection& operator=(const HashMapKeyCollection& _other)
		{
			this->m_pMap = _other.pHashMap_;
			pHashMap_ = _other.pHashMap_;
			return *this;
		}

		~HashMapKeyCollection() noexcept override = default;

		int Size() const override
		{
			return TKeyCollection::Size();
		}

		bool IsEmpty() const override
		{
			return TKeyCollection::IsEmpty();
		}

		TEnumerator Begin() const override
		{
			return MakeShared<HashMapKeyCollectionIterator, TAllocator>(
				pHashMap_->GetOwner(),
				pHashMap_->hashTable_.pHeadBucket_,
				0
			);
		}

		TEnumerator End() const override
		{
			return MakeShared<HashMapKeyCollectionIterator, TAllocator>(
				pHashMap_->GetOwner(),
				pHashMap_->hashTable_.pTailBucket_,
				pHashMap_->hashTable_.pTailBucket_ ? pHashMap_->hashTable_.pTailBucket_->size_ - 1 : -1
			);
		}

		ContainerType GetContainerType() override
		{
			return ContainerType::HashMapKeyCollection;
		}

		THashMap* pHashMap_;
	};

	struct HashMapKeyCollectionIterator final : public TKeyCollectionIterator
	{
		HashMapKeyCollectionIterator(CVoidOwner& _owner,
		                             typename HashTable<TKey, TValue, TAllocator>::TBucket* _pCurrentBucket,
		                             int _currentBucketIndex)
		: TKeyCollectionIterator(_owner, &hashMapIterator_)
		, hashMapIterator_(_owner, _pCurrentBucket, _currentBucketIndex)
		{
		}

		~HashMapKeyCollectionIterator() noexcept override = default;

		THashMapIterator hashMapIterator_;
	};

	struct HashMapValueCollection final : public TValueCollection
	{
		using TEnumerator = SharedPtr<Iterator<TValue, TAllocator>>;
		using TCollection = Collection<TValue, TAllocator>;

		HashMapValueCollection(THashMap* _pHashMap)
		: TMapCollection::ValueCollection(_pHashMap)
		, pHashMap_(_pHashMap)
		{
		}

		~HashMapValueCollection() noexcept override = default;

		HashMapValueCollection& operator=(const HashMapValueCollection& _other)
		{
			this->m_pMap = _other.pHashMap_;
			pHashMap_ = _other.pHashMap_;
			return *this;
		}

		TEnumerator Begin() const override
		{
			return MakeShared<HashMapValueCollectionIterator, TAllocator>(
				pHashMap_->GetOwner(),
				pHashMap_->hashTable_.pHeadBucket_,
				0
			);
		}

		TEnumerator End() const override
		{
			return MakeShared<HashMapValueCollectionIterator, TAllocator>(
				pHashMap_->GetOwner(),
				pHashMap_->hashTable_.pTailBucket_,
				pHashMap_->hashTable_.pTailBucket_ ? pHashMap_->hashTable_.pTailBucket_->size_ - 1 : -1
			);
		}

		ContainerType GetContainerType() override
		{
			return ContainerType::HashMapValueCollection;
		}

		THashMap* pHashMap_;
	};

	struct HashMapValueCollectionIterator final : public TValueCollectionIterator
	{
		HashMapValueCollectionIterator(CVoidOwner& _owner,
		                               typename HashTable<TKey, TValue, TAllocator>::TBucket* _pCurrentBucket,
		                               int _currentBucketIndex)
		: TValueCollectionIterator(_owner, &hashMapIterator_)
		, hashMapIterator_(_owner, _pCurrentBucket, _currentBucketIndex)
		{
		}

		~HashMapValueCollectionIterator() noexcept override = default;

		THashMapIterator hashMapIterator_;
	};

	friend class THashMapIterator;
}; // class HashMap<TKey, TValue, TAllocator>

NS_JC_END
