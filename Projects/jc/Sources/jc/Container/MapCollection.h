/*
	작성자 : 윤정도
*/

#pragma once

#include <jc/Container/Pair.h>
#include <jc/Container/Collection.h>
#include <jc/Container/MapCollectionIterator.h>

NS_JC_BEGIN

/*=====================================================================================
								맵 콜렉션
					      트리 맵, 해쉬 맵 공통 인터페이스 정의
=====================================================================================*/

template <typename TKey, typename TValue, typename TAllocator>
class JC_NOVTABLE MapCollection : public Collection<Pair<TKey, TValue>, TAllocator>
{
	using TKeyValuePair = Pair<TKey, TValue>;
	using TCollection = Collection<TKeyValuePair, TAllocator>;
	using TMapCollection = MapCollection<TKey, TValue, TAllocator>;
	using TMapCollectionIterator = MapCollectionIterator<TKey, TValue, TAllocator>;

public:
	MapCollection() = default;
	~MapCollection() noexcept override = 0;

	// 전방 선언 및 HashMap과 TreeMap에서 접근할 수 있도록 public으로 선언함
	class KeyCollection;
	class ValueCollection;

public:
	virtual bool Insert(const TKeyValuePair& _pair) = 0;
	virtual bool Insert(TKeyValuePair&& _pair) = 0;
	virtual void Clear() = 0;
	virtual bool Exist(const TKey& _key) const = 0;
	virtual TValue& Get(const TKey& _key) const = 0;
	virtual bool Remove(const TKey& _key) = 0;

	CollectionType GetCollectionType() override { return CollectionType::Map; }

	class KeyCollection : public Collection<TKey, TAllocator>
	{
	public:
		using KeyCollection_ = Collection<TKey, TAllocator>;

		KeyCollection(TMapCollection* pMap)
		{
			map_ = pMap;
		}

		~KeyCollection() noexcept override = default;

		int Size() const override
		{
			return map_->Size();
		}

		bool IsEmpty() const override
		{
			return map_->IsEmpty();
		}

		CollectionType GetCollectionType() override { return CollectionType::KeyCollection; }

		TMapCollection* map_;
	};

	class KeyCollectionIterator : public Iterator<TKey, TAllocator>
	{
	public:
		using TKeyIterator = Iterator<TKey, TAllocator>;

		KeyCollectionIterator(CVoidOwner& _owner, TMapCollectionIterator* pIterator)
			: TKeyIterator(_owner)
		{
			mapIterator_ = pIterator;
		}

		~KeyCollectionIterator() noexcept override = default;

		bool HasPrevious() const override
		{
			return mapIterator_->HasPrevious();
		}

		bool HasNext() const override
		{
			return mapIterator_->HasNext();
		}

		TKey& Next() override
		{
			return mapIterator_->Next().key_;
		}

		TKey& Previous() override
		{
			return mapIterator_->Previous().key_;
		}

		TKey& Current() override
		{
			return mapIterator_->Current().key_;
		}

		bool IsBegin() const override
		{
			return mapIterator_->IsBegin();
		}

		bool IsEnd() const override
		{
			return mapIterator_->IsEnd();
		}

		TMapCollectionIterator* mapIterator_;
	};

	class ValueCollection : public Collection<TValue, TAllocator>
	{
	public:
		using TValueCollection = Collection<TValue, TAllocator>;

		ValueCollection(TMapCollection* pMap)
		{
			map_ = pMap;
		}

		~ValueCollection() noexcept override = default;

		int Size() const override
		{
			return map_->Size();
		}

		bool IsEmpty() const override
		{
			return map_->IsEmpty();
		}

		CollectionType GetCollectionType() override { return CollectionType::ValueCollection; }

		TMapCollection* map_;
	};

	class ValueCollectionIterator: public Iterator<TValue, TAllocator>
	{
	public:
		using TValueIterator = Iterator<TValue, TAllocator>;
		
		ValueCollectionIterator(CVoidOwner& _owner, TMapCollectionIterator* pIterator)
			: TValueIterator(_owner)
		{
			mapIterator_ = pIterator;
		}

		~ValueCollectionIterator() noexcept override = default;

		bool HasPrevious() const override
		{
			return mapIterator_->HasPrevious();
		}

		bool HasNext() const override
		{
			return mapIterator_->HasNext();
		}

		TValue& Next() override
		{
			return mapIterator_->Next().value_;
		}

		TValue& Previous() override
		{
			return mapIterator_->Previous().value_;
		}

		TValue& Current() override
		{
			return mapIterator_->Current().value_;
		}

		bool IsBegin() const override
		{
			return mapIterator_->IsBegin();
		}

		bool IsEnd() const override
		{
			return mapIterator_->IsEnd();
		}

		TMapCollectionIterator* mapIterator_;
	};
};

template <typename TKey, typename TValue, typename TAllocator>
MapCollection<TKey, TValue, TAllocator>::~MapCollection() noexcept {}

NS_END
