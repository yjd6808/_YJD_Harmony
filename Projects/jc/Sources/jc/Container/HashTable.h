/*
 * 작성자: 윤정도
 * 생성일: 8/14/2023 9:33:27 AM
 * =====================
 * 해쉬맵만 쓰다가 해쉬셋이 필요해져서 하나의 클래스로 표현하기 위해 HashTable을 정의함
 */


#pragma once

#include <jc/Container/HashTable_Bucket.h>

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
	HashTable(int _capacity = TABLE_DEFAULT_CAPACITY)
	: pTable_(nullptr)
	, pHeadBucket_(nullptr)
	, pTailBucket_(nullptr)
	, capacity_(_capacity)
	, size_(0)
	{
		if (capacity_ > 0)
		{
			int allocatedSize = 0;
			pTable_ = TAllocator::template AllocateDynamic<TBucket*>(sizeof(TBucket) * _capacity, allocatedSize);
			Memory::PlacementNewArray(pTable_, _capacity);
		}
	}

	HashTable(const THashTable& _other)
	: THashTable(_other.capacity_)
	{
		operator=(_other);
	}

	HashTable(THashTable&& _other) noexcept
	: pTable_(nullptr)
	, pHeadBucket_(nullptr)
	, pTailBucket_(nullptr)
	{
		operator=(Move(_other));
	}

	// 이니셜라이저로 초기화 하는 경우 보통 더 확장안시킬 확률이 크므로.. 맞춤형으로 가자.
	HashTable(std::initializer_list<TKey> _ilist)
	: THashTable(static_cast<int>(_ilist.size()) + 1)
	{
		operator=(_ilist);
	}

	~HashTable() noexcept
	{
		Clear();

		JC_PLACEMENT_DELETE_ARRAY_SAFE(pTable_, capacity_);
		JC_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(pTable_, capacity_ * sizeof(TBucket));
	}

public:
	THashTable& operator=(const THashTable& _other)
	{
		Clear();
		ExpandIfNeeded(_other.size_);

		TBucket* pOtherBucketCurrent = _other.pHeadBucket_;

		while (pOtherBucketCurrent != nullptr)
		{
			for (int i = 0; i < pOtherBucketCurrent->size_; ++i)
			{
				TBucketNode& node = pOtherBucketCurrent->GetAt(i);
				Int32U bucketIndex = BucketIndex(node.hash_);
				TBucket& bucket = pTable_[bucketIndex];

				if (bucket.IsEmpty())
				{
					PushBackNewBucket(&bucket);
				}

				bucket.PushBack(node);
			}

			pOtherBucketCurrent = pOtherBucketCurrent->pNext_;
		}

		size_ = _other.size_;
		return *this;
	}

	THashTable& operator=(THashTable&& _other) noexcept
	{
		Clear();

		JC_PLACEMENT_DELETE_ARRAY_SAFE(pTable_, capacity_);
		JC_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(pTable_, sizeof(TBucket) * capacity_);

		size_ = _other.size_;
		capacity_ = _other.capacity_;
		pTable_ = _other.pTable_;
		pHeadBucket_ = _other.pHeadBucket_;
		pTailBucket_ = _other.pTailBucket_;

		_other.pTable_ = nullptr;
		_other.pHeadBucket_ = nullptr;
		_other.pTailBucket_ = nullptr;
		_other.size_ = 0;

		return *this;
	}

	THashTable& operator=(std::initializer_list<TKey> _ilist)
	{
		Clear();
		ExpandIfNeeded(static_cast<int>(_ilist.size()));

		for (auto it = _ilist.begin(); it != _ilist.end(); ++it)
		{
			Insert((*it));
		}

		return *this;
	}

	template <typename Ky>
	bool Insert(Ky&& _key)
	{
		if (IsFull())
		{
			Expand(capacity_ * TABLE_EXPANDING_FACTOR);
		}

		if (Exist(_key))
		{
			return false;
		}

		Int32U hashValue = Hash(_key);
		Int32U bucketIndex = BucketIndex(hashValue);
		TBucket& bucket = pTable_[bucketIndex];

		if (bucket.IsEmpty())
		{
			PushBackNewBucket(&bucket);
		}

		bucket.EmplaceBack(Forward<Ky>(_key), hashValue);
		++size_;
		return true;
	}

	template <typename Ky>
	bool Exist(const Ky& _key) const
	{
		if (pTable_ == nullptr)
		{
			return false;
		}

		return pTable_[HashBucket(_key)].Exist(_key);
	}

	template <typename Ky>
	bool Remove(const Ky& _key)
	{
		TBucket& bucket = pTable_[HashBucket(_key)];

		if (!bucket.Remove(_key))
		{
			return false;
		}

		// 버킷이 비었으면 연결을 끊어준다.
		if (bucket.IsEmpty())
		{
			DisjointBucket(&bucket);
		}

		--size_;
		return true;
	}

	void DisjointBucket(TBucket* _pBucket)
	{
		if (_pBucket == pHeadBucket_)
		{
			pHeadBucket_ = pHeadBucket_->pNext_;

			if (pHeadBucket_ == nullptr)
			{
				pTailBucket_ = nullptr;
			}
			else
			{
				pHeadBucket_->pPrevious_ = nullptr;
			}
		}
		else if (_pBucket == pTailBucket_)
		{
			pTailBucket_ = pTailBucket_->pPrevious_;

			if (pTailBucket_ == nullptr)
			{
				pHeadBucket_ = nullptr;
			}
			else
			{
				pTailBucket_->pNext_ = nullptr;
			}
		}
		else
		{
			ConnectBucket(_pBucket->pPrevious_, _pBucket->pNext_);
		}

		_pBucket->pPrevious_ = nullptr;
		_pBucket->pNext_ = nullptr;
	}

	void Clear() noexcept
	{
		if (size_ == 0)
		{
			return;
		}

		TBucket* pCurrent = pHeadBucket_;

		while (pCurrent != nullptr)
		{
			TBucket* pTemp = pCurrent;
			pTemp->Clear();
			pCurrent = pCurrent->pNext_;
		}

		size_ = 0;
		pHeadBucket_ = nullptr;
		pTailBucket_ = nullptr;
	}

	bool Valid() const
	{
		return pTable_ != nullptr;
	}

	TBucket* Bucket(int _index) const
	{
		if (_index < 0 || _index >= capacity_)
		{
			jc_assert(false);
			return nullptr;
		}

		return &pTable_[_index];
	}

	int BucketCount()
	{
		int count = 0;

		for (int i = 0; i < capacity_; ++i)
		{
			if (pTable_[i].IsEmpty())
			{
				continue;
			}

			++count;
		}

		return count;
	}

	// ==========================================
	// 동적할당 안하고 해쉬맵 순회할 수 있도록 기능 구현
	// ==========================================
	template <typename Consumer>
	void ForEach(Consumer&& consumer)
	{
		TBucket* pCurrentBucket = pHeadBucket_;

		while (pCurrentBucket != nullptr)
		{
			for (int i = 0; i < pCurrentBucket->size_; ++i)
			{
				TBucketNode& node = pCurrentBucket->GetAt(i);
				consumer(node.data_);
			}

			pCurrentBucket = pCurrentBucket->pNext_;
		}
	}

	void ForEachDelete()
	{
		if constexpr (!IsPointerType_v<TKey>)
		{
			jc_assert(false);
			return;
		}

		TBucket* pCurrentBucket = pHeadBucket_;

		while (pCurrentBucket != nullptr)
		{
			for (int i = 0; i < pCurrentBucket->size_; ++i)
			{
				TBucketNode& node = pCurrentBucket->GetAt(i);
				delete node.data_;
			}

			pCurrentBucket = pCurrentBucket->pNext_;
		}

		Clear();
	}

	void Expand(int _capacity)
	{
		// 0 용량 해쉬맵 기능 추가때문에
		if (_capacity == 0)
		{
			_capacity = 4;
		}

		jc_assert_msg(_capacity > capacity_, "이전 해쉬맵 크기보다 커야합니다.");

		int allocatedSize = 0;
		TBucket* pNewTable = TAllocator::template AllocateDynamic<TBucket*>(sizeof(TBucket) * _capacity, allocatedSize);
		Memory::PlacementNewArray(pNewTable, _capacity);

		const int previousCapacity = capacity_;

		capacity_ = _capacity;
		pHeadBucket_ = nullptr;
		pTailBucket_ = nullptr;

		for (int i = 0; i < previousCapacity; ++i)
		{
			if (pTable_[i].size_ == 0)
			{
				continue;
			}

			TBucket& previousBucket = pTable_[i];

			// 기존 버킷을 순회하며 새로운 버킷에 데이터를 담아준다.
			for (int j = 0; j < previousBucket.size_; ++j)
			{
				TBucketNode& bucketNode = previousBucket.GetAt(j);
				Int32U bucketIndex = BucketIndex(bucketNode.hash_);

				if (pNewTable[bucketIndex].IsEmpty())
				{
					PushBackNewBucket(&pNewTable[bucketIndex]);
				}

				pNewTable[bucketIndex].PushBack(Move(bucketNode));
			}
		}

		JC_PLACEMENT_DELETE_ARRAY_SAFE(pTable_, previousCapacity);
		JC_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(pTable_, sizeof(TBucket) * previousCapacity);
		pTable_ = pNewTable;
	}

	bool ExpandIfNeeded(int _size)
	{
		if (_size <= 0 || _size < capacity_)
		{
			return false;
		}

		const int expandedCapacity = CalculateExpandCapacity(_size);
		Expand(expandedCapacity);
		return true;
	}

	int Size() const
	{
		return size_;
	}

	int Capacity() const
	{
		return capacity_;
	}

protected:
	/// <summary>
	/// 전달받은 사이즈 크기에 맞는 배열 크기를 반환해준다.
	/// </summary>
	int CalculateExpandCapacity(int _size) const
	{
		if (_size < capacity_)
		{
			return capacity_;
		}

		int expectedCapacity = capacity_;

		while (true)
		{
			expectedCapacity *= TABLE_EXPANDING_FACTOR;

			if (expectedCapacity > _size)
			{
				break;
			}
		}

		return expectedCapacity;
	}

	static void ConnectBucket(TBucket* _pLeft, TBucket* _pRight) noexcept
	{
		if (_pLeft)
		{
			_pLeft->pNext_ = _pRight;
		}

		if (_pRight)
		{
			_pRight->pPrevious_ = _pLeft;
		}
	}

	/// <summary>
	/// bucket 바로 전에 다른 bucket을 삽입한다.
	/// </summary>
	void PushBackNewBucket(TBucket* _pBucket)
	{
		if (pHeadBucket_ == nullptr)
		{
			pHeadBucket_ = _pBucket;
			pTailBucket_ = _pBucket;
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

		ConnectBucket(pTailBucket_, _pBucket);
		pTailBucket_ = _pBucket;
	}

	Int32U BucketIndex(const Int32U _hash) const
	{
		return _hash % capacity_;
	}

	template <typename Ky>
	Int32U Hash(const Ky& _key) const
	{
		return THasher()(_key);
	}

	template <typename Ky>
	Int32U HashBucket(const Ky& _key) const
	{
		return BucketIndex(Hash(_key));
	}

	bool IsFull() const
	{
		return size_ == capacity_;
	}

	static constexpr Int32U TABLE_EXPANDING_FACTOR = 4; // 테이블 크기만큼 데이터가 들어가면 확장하는데 몇배나 확장할 지
	static constexpr Int32U TABLE_DEFAULT_CAPACITY = 16; // 테이블 초기 크기

protected:
	TBucket* pTable_;
	TBucket* pHeadBucket_;
	TBucket* pTailBucket_;
	Int32 capacity_;
	Int32 size_;

	template <typename, typename>
	friend class HashSet;

	template <typename, typename>
	friend class HashSetIterator;
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
	 *	 *
	 *	 *
	 * ==================================================================== */

public:
	using THashTable = HashTable<TKey, TValue, TAllocator>;
	using THasher = Hasher<TKey>;
	using TBucket = Bucket<TKey, TValue, TAllocator>;
	using TBucketNode = BucketNode<Pair<TKey, TValue>>;
	using TKeyValuePair = Pair<TKey, TValue>;

	HashTable(int _capacity = TABLE_DEFAULT_CAPACITY)
	: pTable_(nullptr)
	, pHeadBucket_(nullptr)
	, pTailBucket_(nullptr)
	, capacity_(_capacity)
	, size_(0)
	{
		if (capacity_ > 0)
		{
			int allocatedSize = 0;
			pTable_ = TAllocator::template AllocateDynamic<TBucket*>(sizeof(TBucket) * _capacity, allocatedSize);
			Memory::PlacementNewArray(pTable_, _capacity);
		}
	}

	HashTable(const THashTable& _other)
	: THashTable(_other.capacity_)
	{
		operator=(_other);
	}

	HashTable(THashTable&& _other) noexcept
	: pTable_(nullptr)
	, pHeadBucket_(nullptr)
	, pTailBucket_(nullptr)
	{
		operator=(Move(_other));
	}

	// 이니셜라이저로 초기화 하는 경우 보통 더 확장안시킬 확률이 크므로.. 맞춤형으로 가자.
	HashTable(std::initializer_list<TKeyValuePair> _ilist)
	: THashTable(static_cast<int>(_ilist.size()) + 1)
	{
		operator=(_ilist);
	}

	~HashTable() noexcept
	{
		Clear();

		JC_PLACEMENT_DELETE_ARRAY_SAFE(pTable_, capacity_);
		JC_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(pTable_, capacity_ * sizeof(TBucket));
	}

public:
	THashTable& operator=(const THashTable& _other)
	{
		Clear();
		ExpandIfNeeded(_other.size_);

		TBucket* pOtherBucketCurrent = _other.pHeadBucket_;

		while (pOtherBucketCurrent != nullptr)
		{
			for (int i = 0; i < pOtherBucketCurrent->size_; ++i)
			{
				TBucketNode& node = pOtherBucketCurrent->GetAt(i);
				Int32U bucketIndex = BucketIndex(node.hash_);
				TBucket& bucket = pTable_[bucketIndex];

				if (bucket.IsEmpty())
				{
					PushBackNewBucket(&bucket);
				}

				bucket.PushBack(node);
			}

			pOtherBucketCurrent = pOtherBucketCurrent->pNext_;
		}

		size_ = _other.size_;
		return *this;
	}

	THashTable& operator=(THashTable&& _other) noexcept
	{
		Clear();

		JC_PLACEMENT_DELETE_ARRAY_SAFE(pTable_, capacity_);
		JC_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(pTable_, sizeof(TBucket) * capacity_);

		size_ = _other.size_;
		capacity_ = _other.capacity_;
		pTable_ = _other.pTable_;
		pHeadBucket_ = _other.pHeadBucket_;
		pTailBucket_ = _other.pTailBucket_;

		_other.pTable_ = nullptr;
		_other.pHeadBucket_ = nullptr;
		_other.pTailBucket_ = nullptr;
		_other.size_ = 0;

		return *this;
	}

	THashTable& operator=(std::initializer_list<TKeyValuePair> _ilist)
	{
		Clear();
		ExpandIfNeeded(static_cast<int>(_ilist.size()));

		for (auto it = _ilist.begin(); it != _ilist.end(); ++it)
		{
			Insert(Move(it->key_), Move(it->value_));
		}

		return *this;
	}

	TValue& operator[](const TKey& _key)
	{
		return Get(_key);
	}

	template <typename Ky, typename Vy>
	bool Insert(Ky&& _key, Vy&& _value)
	{
		if (IsFull())
		{
			Expand(capacity_ * TABLE_EXPANDING_FACTOR);
		}

		if (Exist(_key))
		{
			return false;
		}

		Int32U hashValue = Hash(_key);
		Int32U bucketIndex = BucketIndex(hashValue);
		TBucket& bucket = pTable_[bucketIndex];

		if (bucket.IsEmpty())
		{
			PushBackNewBucket(&bucket);
		}

		bucket.EmplaceBack(TKeyValuePair{
			                   static_cast<TKey>(Forward<Ky>(_key)), static_cast<TValue>(Forward<Vy>(_value))
		                   }, hashValue);
		++size_;
		return true;
	}

	template <typename Ky>
	bool Exist(const Ky& _key) const
	{
		if (pTable_ == nullptr)
		{
			return false;
		}

		return pTable_[HashBucket(_key)].Exist(_key);
	}

	template <typename Ky>
	TValue* Find(const Ky& _key) const
	{
		if (pTable_ == nullptr)
		{
			return nullptr;
		}

		TValue* pValue = pTable_[HashBucket(_key)].Find(_key);

		if (pValue == nullptr)
		{
			return nullptr;
		}

		return pValue;
	}

	template <typename Ky>
	TValue& Get(const Ky& _key) const
	{
		if (pTable_ == nullptr)
		{
			throw InvalidOperationException("초기화 되지 않은 해쉬맵입니다.");
		}

		TValue* pValue = pTable_[HashBucket(_key)].Find(_key);

		if (pValue == nullptr)
		{
			throw InvalidArgumentException("해당 키값에 대응하는 값이 존재하지 않습니다.");
		}

		return *pValue;
	}

	template <typename Ky>
	bool Remove(const Ky& _key)
	{
		TBucket& bucket = pTable_[HashBucket(_key)];

		if (!bucket.Remove(_key))
		{
			return false;
		}

		// 버킷이 비었으면 연결을 끊어준다.
		if (bucket.IsEmpty())
		{
			DisjointBucket(&bucket);
		}

		--size_;
		return true;
	}

	void DisjointBucket(TBucket* _pBucket)
	{
		if (_pBucket == pHeadBucket_)
		{
			pHeadBucket_ = pHeadBucket_->pNext_;

			if (pHeadBucket_ == nullptr)
			{
				pTailBucket_ = nullptr;
			}
			else
			{
				pHeadBucket_->pPrevious_ = nullptr;
			}
		}
		else if (_pBucket == pTailBucket_)
		{
			pTailBucket_ = pTailBucket_->pPrevious_;

			if (pTailBucket_ == nullptr)
			{
				pHeadBucket_ = nullptr;
			}
			else
			{
				pTailBucket_->pNext_ = nullptr;
			}
		}
		else
		{
			ConnectBucket(_pBucket->pPrevious_, _pBucket->pNext_);
		}

		// 연결리스트는 발견된 노드를 삭제하기 때문에 Next, Previous를 null로 설정할 필요가 없었는데
		// 해쉬테이블은 노드를 삭제한게 아니기 땜에 Next, Previous를 null로 설정해놔야한다. ㅠㅠ
		// 작성된 코드에는 문제가 없었고.. 추가를 해줬어야했다 ㅠㅠ 논리적 오류가 있을 줄 알앗는데.. 아니었다.
		// 1시간 30분동안이나 디버깅 해서 겨우 찾음 ㅠㅠ;
		_pBucket->pNext_ = nullptr;
		_pBucket->pPrevious_ = nullptr;
	}

	void Clear() noexcept
	{
		if (size_ == 0)
		{
			return;
		}

		TBucket* pCurrent = pHeadBucket_;

		while (pCurrent != nullptr)
		{
			TBucket* pTemp = pCurrent;
			pTemp->Clear();
			pCurrent = pCurrent->pNext_;
		}

		size_ = 0;
		pHeadBucket_ = nullptr;
		pTailBucket_ = nullptr;
	}

	bool Valid() const
	{
		return pTable_ != nullptr;
	}

	TBucket* Bucket(int _index) const
	{
		if (_index < 0 || _index >= capacity_)
		{
			jc_assert(false);
			return nullptr;
		}

		return &pTable_[_index];
	}

	int BucketCount()
	{
		int count = 0;

		for (int i = 0; i < capacity_; ++i)
		{
			if (pTable_[i].IsEmpty())
			{
				continue;
			}

			++count;
		}

		return count;
	}

	// ==========================================
	// 동적할당 안하고 해쉬맵 순회할 수 있도록 기능 구현
	// ==========================================
	template <typename Consumer>
	void ForEach(Consumer&& _consumer)
	{
		TBucket* pCurrentBucket = pHeadBucket_;

		while (pCurrentBucket != nullptr)
		{
			for (int i = 0; i < pCurrentBucket->size_; ++i)
			{
				TBucketNode& node = pCurrentBucket->GetAt(i);
				_consumer(node.data_);
			}

			pCurrentBucket = pCurrentBucket->pNext_;
		}
	}

	template <typename Consumer>
	void ForEachKey(Consumer&& _consumer)
	{
		TBucket* pCurrentBucket = pHeadBucket_;

		while (pCurrentBucket != nullptr)
		{
			for (int i = 0; i < pCurrentBucket->size_; ++i)
			{
				TBucketNode& node = pCurrentBucket->GetAt(i);
				_consumer(node.data_.key_);
			}

			pCurrentBucket = pCurrentBucket->pNext_;
		}
	}

	template <typename Consumer>
	void ForEachValue(Consumer&& _consumer)
	{
		TBucket* pCurrentBucket = pHeadBucket_;

		while (pCurrentBucket != nullptr)
		{
			for (int i = 0; i < pCurrentBucket->size_; ++i)
			{
				TBucketNode& node = pCurrentBucket->GetAt(i);
				_consumer(node.data_.value_);
			}

			pCurrentBucket = pCurrentBucket->pNext_;
		}
	}

	// Value들만 순회해서 삭제하는 작업
	// 자주 사용해서 그냥 라이브러리에 박음
	void ForEachValueDelete()
	{
		if constexpr (!IsPointerType_v<TValue>)
		{
			jc_assert(false);
			return;
		}

		TBucket* pCurrentBucket = pHeadBucket_;

		while (pCurrentBucket != nullptr)
		{
			for (int i = 0; i < pCurrentBucket->size_; ++i)
			{
				TBucketNode& node = pCurrentBucket->GetAt(i);
				delete node.data_.value_;
			}

			pCurrentBucket = pCurrentBucket->pNext_;
		}
	}

	void ForEachValueRelease()
	{
		if constexpr (!IsPointerType_v<TValue>)
		{
			jc_assert(false);
			return;
		}

		TBucket* pCurrentBucket = pHeadBucket_;

		while (pCurrentBucket != nullptr)
		{
			for (int i = 0; i < pCurrentBucket->size_; ++i)
			{
				TBucketNode& node = pCurrentBucket->GetAt(i);
				node.data_.value_->Release();
			}

			pCurrentBucket = pCurrentBucket->pNext_;
		}
	}

	void Expand(int _capacity)
	{
		// 0 용량 해쉬맵 기능 추가때문에
		if (_capacity == 0)
		{
			_capacity = 4;
		}

		jc_assert_msg(_capacity > capacity_, "이전 해쉬맵 크기보다 커야합니다.");

		int allocatedSize = 0;
		TBucket* pNewTable = TAllocator::template AllocateDynamic<TBucket*>(sizeof(TBucket) * _capacity, allocatedSize);
		Memory::PlacementNewArray(pNewTable, _capacity);

		const int previousCapacity = capacity_;

		capacity_ = _capacity;
		pHeadBucket_ = nullptr;
		pTailBucket_ = nullptr;

		for (int i = 0; i < previousCapacity; ++i)
		{
			if (pTable_[i].size_ == 0)
			{
				continue;
			}

			TBucket& previousBucket = pTable_[i];

			// 기존 버킷을 순회하며 새로운 버킷에 데이터를 담아준다.
			for (int j = 0; j < previousBucket.size_; ++j)
			{
				TBucketNode& bucketNode = previousBucket.GetAt(j);
				Int32U bucketIndex = BucketIndex(bucketNode.hash_);

				if (pNewTable[bucketIndex].IsEmpty())
				{
					PushBackNewBucket(&pNewTable[bucketIndex]);
				}

				pNewTable[bucketIndex].PushBack(Move(bucketNode));
			}
		}

		JC_PLACEMENT_DELETE_ARRAY_SAFE(pTable_, previousCapacity);
		JC_ALLOCATOR_DYNAMIC_DEALLOCATE_SAFE(pTable_, sizeof(TBucket) * previousCapacity);
		pTable_ = pNewTable;
	}

	bool ExpandIfNeeded(int _size)
	{
		if (_size <= 0 || _size < capacity_)
		{
			return false;
		}

		const int expandedCapacity = CalculateExpandCapacity(_size);
		Expand(expandedCapacity);
		return true;
	}

	int Size() const
	{
		return size_;
	}

	int Capacity() const
	{
		return capacity_;
	}

protected:
	/// <summary>
	/// 전달받은 사이즈 크기에 맞는 배열 크기를 반환해준다.
	/// </summary>
	int CalculateExpandCapacity(int _size) const
	{
		if (_size < capacity_)
		{
			return capacity_;
		}

		int expectedCapacity = capacity_;

		while (true)
		{
			expectedCapacity *= TABLE_EXPANDING_FACTOR;

			if (expectedCapacity > _size)
			{
				break;
			}
		}

		return expectedCapacity;
	}

	static void ConnectBucket(TBucket* _pLeft, TBucket* _pRight) noexcept
	{
		if (_pLeft)
		{
			_pLeft->pNext_ = _pRight;
		}

		if (_pRight)
		{
			_pRight->pPrevious_ = _pLeft;
		}
	}

	/// <summary>
	/// bucket 바로 전에 다른 bucket을 삽입한다.
	/// </summary>
	void PushBackNewBucket(TBucket* _pBucket)
	{
		if (pHeadBucket_ == nullptr)
		{
			pHeadBucket_ = _pBucket;
			pTailBucket_ = _pBucket;
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

		ConnectBucket(pTailBucket_, _pBucket);
		pTailBucket_ = _pBucket;
	}

	Int32U BucketIndex(const Int32U _hash) const
	{
		return _hash % capacity_;
	}

	template <typename Ky>
	Int32U Hash(const Ky& _key) const
	{
		return THasher()(_key);
	}

	template <typename Ky>
	Int32U HashBucket(const Ky& _key) const
	{
		return BucketIndex(Hash(_key));
	}

	bool IsFull() const
	{
		return size_ == capacity_;
	}

	static constexpr Int32U TABLE_EXPANDING_FACTOR = 4; // 테이블 크기만큼 데이터가 들어가면 확장하는데 몇배나 확장할 지
	static constexpr Int32U TABLE_DEFAULT_CAPACITY = 16; // 테이블 초기 크기

protected:
	TBucket* pTable_;
	TBucket* pHeadBucket_;
	TBucket* pTailBucket_;
	Int32 capacity_;
	Int32 size_;

	template <typename, typename, typename>
	friend class HashMap;

	template <typename, typename, typename>
	friend class HashMapIterator;

	template <typename, typename>
	friend class Properties;

	template <typename, typename>
	friend class CPropertiesIterator;
}; // class HashTable<TKey, TValue, TAllocator>

#pragma endregion

NS_JC_END
