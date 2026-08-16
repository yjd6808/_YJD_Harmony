/*
 *	작성자 : 윤정도
 */

#pragma once

#include "jc/Container/HashTable.h"
#include "jc/Container/HashMapIterator.h"

NS_JC_BEGIN
template <typename TKey, typename TValue, typename TAllocator = CDefaultAllocator>
class HashMap
{
	using TKeyValuePair = Pair<TKey, TValue>;
	using THashMap = HashMap<TKey, TValue, TAllocator>;
	using THashMapIterator = HashMapIterator<TKey, TValue, TAllocator>;

public:
	HashMap(int _capacity = HashTable<TKey, TValue, TAllocator>::TABLE_DEFAULT_CAPACITY)
	: hashTable_(_capacity)
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

	~HashMap() noexcept = default;

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

	bool Insert(const TKeyValuePair& _pair)
	{
		return hashTable_.Insert(_pair.key_, _pair.value_);
	}

	bool Insert(TKeyValuePair&& _pair)
	{
		return hashTable_.Insert(Move(_pair.key_), Move(_pair.value_));
	}

	template <typename = DefaultEnableIf_t<IsStringType_v<TKey>>>
	bool Exist(const char* _pKey) const
	{
		return hashTable_.Exist(_pKey);
	}

	bool Exist(const TKey& _key) const
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

	TKey* AnyKey() const
	{
		return hashTable_.AnyKey();
	}

	TValue* AnyValue() const
	{
		return hashTable_.AnyValue();
	}

	TValue& Get(const TKey& _key) const
	{
		return hashTable_.Get(_key);
	}

	template <typename = DefaultEnableIf_t<IsStringType_v<TKey>>>
	bool Remove(const char* _pKey)
	{
		return hashTable_.Remove(_pKey);
	}

	bool Remove(const TKey& _key)
	{
		return hashTable_.Remove(_key);
	}

	template <typename = DefaultEnableIf_t<IsStringType_v<TKey>>>
	bool TryPop(const char* _pKey, TValue* _pOut)
	{
		return hashTable_.TryPop(_pKey, _pOut);
	}

	bool TryPop(const TKey& _key, TValue* _pOut)
	{
		return hashTable_.TryPop(_key, _pOut);
	}

	void Clear() noexcept
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

	int Size() const
	{
		return hashTable_.Size();
	}

	bool IsEmpty() const
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
	void ForEach(Consumer&& _consumer) const
	{
		hashTable_.ForEach(Forward<Consumer>(_consumer));
	}

	template <typename Consumer>
	void ForEachKey(Consumer&& _consumer) const
	{
		hashTable_.ForEachKey(Forward<Consumer>(_consumer));
	}

	template <typename Consumer>
	void ForEachValue(Consumer&& _consumer) const
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

	THashMapIterator Begin() const
	{
		return THashMapIterator(hashTable_.pHeadBucket_, 0);
	}

	THashMapIterator End() const
	{
		return THashMapIterator(
			hashTable_.pTailBucket_,
			hashTable_.pTailBucket_ ? hashTable_.pTailBucket_->size_ - 1 : -1
		);
	}

	auto Keys() const
	{
		return HashMapKeyCollection(const_cast<THashMap*>(this));
	}

	auto Values() const
	{
		return HashMapValueCollection(const_cast<THashMap*>(this));
	}

protected:
	HashTable<TKey, TValue, TAllocator> hashTable_;

public:
	struct HashMapKeyCollection
	{
		using TBucket		 = ::jc::Bucket<TKey, TValue, TAllocator>;

		HashMapKeyCollection(THashMap* _pHashMap)
			: pHashMap_(_pHashMap)
		{
		}

		HashMapKeyCollection& operator=(const HashMapKeyCollection& _other)
		{
			pHashMap_ = _other.pHashMap_;
			return *this;
		}

		int Size() const
		{
			return pHashMap_->Size();
		}

		bool IsEmpty() const
		{
			return pHashMap_->IsEmpty();
		}

		struct Iterator
		{
			using TMapIterator = HashMapIterator<TKey, TValue, TAllocator>;

			Iterator(TBucket* _pCurrentBucket, int _currentBucketIndex)
				: mapIterator_(_pCurrentBucket, _currentBucketIndex)
			{
			}

			bool HasNext() const
			{
				return mapIterator_.HasNext();
			}

			bool HasPrevious() const
			{
				return mapIterator_.HasPrevious();
			}

			TKey& Next()
			{
				return mapIterator_.Next().key_;
			}

			TKey& Previous()
			{
				return mapIterator_.Previous().key_;
			}

			TKey& Current()
			{
				return mapIterator_.Current().key_;
			}

			bool IsEnd() const
			{
				return mapIterator_.IsEnd();
			}

			bool IsBegin() const
			{
				return mapIterator_.IsBegin();
			}

		private:
			TMapIterator mapIterator_;
		};

		auto Begin() const
		{
			return Iterator(pHashMap_->hashTable_.pHeadBucket_, 0);
		}

		auto End() const
		{
			return Iterator(
				pHashMap_->hashTable_.pTailBucket_,
				pHashMap_->hashTable_.pTailBucket_ ? pHashMap_->hashTable_.pTailBucket_->size_ - 1 : -1
			);
		}

		bool Exist(const TKey& _value) const
		{
			auto it = Begin();
			while (it.HasNext())
			{
				if (it.Next() == _value)
				{
					return true;
				}
			}
			return false;
		}

		TKey* First()
		{
			auto it = Begin();
			if (!it.HasNext())
			{
				return nullptr;
			}

			return AddressOf(it.Next());
		}

		TKey* Last()
		{
			auto it = End();
			if (!it.HasPrevious())
			{
				return nullptr;
			}

			return AddressOf(it.Previous());
		}

		THashMap* pHashMap_;
	};

	struct HashMapValueCollection
	{
		using TBucket		 = ::jc::Bucket<TKey, TValue, TAllocator>;

		HashMapValueCollection(THashMap* _pHashMap)
			: pHashMap_(_pHashMap)
		{
		}

		HashMapValueCollection& operator=(const HashMapValueCollection& _other)
		{
			pHashMap_ = _other.pHashMap_;
			return *this;
		}

		int Size() const
		{
			return pHashMap_->Size();
		}

		bool IsEmpty() const
		{
			return pHashMap_->IsEmpty();
		}

		struct Iterator
		{
			using TMapIterator = HashMapIterator<TKey, TValue, TAllocator>;

			Iterator(TBucket* _pCurrentBucket, int _currentBucketIndex)
				: mapIterator_(_pCurrentBucket, _currentBucketIndex)
			{
			}

			bool HasNext() const
			{
				return mapIterator_.HasNext();
			}

			bool HasPrevious() const
			{
				return mapIterator_.HasPrevious();
			}

			TValue& Next()
			{
				return mapIterator_.Next().value_;
			}

			TValue& Previous()
			{
				return mapIterator_.Previous().value_;
			}

			TValue& Current()
			{
				return mapIterator_.Current().value_;
			}

			bool IsEnd() const
			{
				return mapIterator_.IsEnd();
			}

			bool IsBegin() const
			{
				return mapIterator_.IsBegin();
			}

		private:
			TMapIterator mapIterator_;
		};

		auto Begin() const
		{
			return Iterator(pHashMap_->hashTable_.pHeadBucket_, 0);
		}

		auto End() const
		{
			return Iterator(
				pHashMap_->hashTable_.pTailBucket_,
				pHashMap_->hashTable_.pTailBucket_ ? pHashMap_->hashTable_.pTailBucket_->size_ - 1 : -1
			);
		}

		bool Exist(const TValue& _value) const
		{
			auto it = Begin();
			while (it.HasNext())
			{
				if (it.Next() == _value)
				{
					return true;
				}
			}
			return false;
		}

		TValue* First()
		{
			auto it = Begin();
			if (!it.HasNext())
			{
				return nullptr;
			}

			return AddressOf(it.Next());
		}

		TValue* Last()
		{
			auto it = End();
			if (!it.HasPrevious())
			{
				return nullptr;
			}

			return AddressOf(it.Previous());
		}

		THashMap* pHashMap_;
	};

	friend class THashMapIterator;
}; // class HashMap<TKey, TValue, TAllocator>

NS_END