/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/Pair.h>
#include <JCore/Container/Collection.h>
#include <JCore/Container/MapCollectionIterator.h>

NS_JC_BEGIN

/*=====================================================================================
									맵 콜렉션
					      트리 맵, 해쉬 맵 공통 인터페이스 정의
=====================================================================================*/

template <typename TKey, typename TValue, typename TAllocator>
class JCORE_NOVTABLE MapCollection : public Collection<Pair<TKey, TValue>, TAllocator>
{
	using TKeyValuePair			 = Pair<TKey, TValue>;
	using TCollection			 = Collection<TKeyValuePair, TAllocator>;
	using TMapCollection		 = MapCollection<TKey, TValue, TAllocator>;
	using TMapCollectionIterator = MapCollectionIterator<TKey, TValue, TAllocator>;


public:
	MapCollection() = default;
	~MapCollection() noexcept override = 0;

	// 전방 선언 및 HashMap과 TreeMap에서 접근할 수 있도록 public으로 선언함
	struct KeyCollection;
	struct ValueCollection;
public:
	virtual bool Insert(const TKeyValuePair& pair) = 0;
	virtual bool Insert(TKeyValuePair&& pair) = 0;
	virtual void Clear() = 0;
	virtual bool Exist(const TKey& key) const = 0;
	virtual TValue& Get(const TKey& key) const = 0;
	virtual bool Remove(const TKey& key) = 0;

	CollectionType GetCollectionType() override { return CollectionType::Map; }

	struct KeyCollection : public Collection<TKey, TAllocator>
	{
		using TkeyCollection = Collection<TKey, TAllocator>;

		KeyCollection(TMapCollection* map) {
			m_pMap = map;
		}

		~KeyCollection() noexcept override = default;

		int Size() const override {
			return m_pMap->Size();
		}

		bool IsEmpty() const override {
			return m_pMap->IsEmpty();
		}

		CollectionType GetCollectionType() override { return CollectionType::KeyCollection; }

		TMapCollection* m_pMap;
	};

	struct KeyCollectionIterator : public Iterator<TKey, TAllocator>
	{
		using TKeyIterator = Iterator<TKey, TAllocator>;

		KeyCollectionIterator(VoidOwner& owner, TMapCollectionIterator* iterator) 
			: TKeyIterator(owner)
		{
			m_pMapIterator = iterator;
		}

		~KeyCollectionIterator() noexcept override = default;


		bool HasPrevious() const override {
			return m_pMapIterator->HasPrevious();
		}

		bool HasNext() const override {
			return m_pMapIterator->HasNext();
		}

		TKey& Next() override {
			return m_pMapIterator->Next().Key;
		}

		TKey& Previous() override {
			return m_pMapIterator->Previous().Key;
		}

		TKey& Current() override {
			return m_pMapIterator->Current().Key;
		}

		bool IsBegin() const override {
			return m_pMapIterator->IsBegin();
		}

		bool IsEnd() const override {
			return m_pMapIterator->IsEnd();
		}

		TMapCollectionIterator* m_pMapIterator;
	};

	struct ValueCollection : public Collection<TValue, TAllocator>
	{
		using TValueCollection = Collection<TValue, TAllocator>;

		ValueCollection(TMapCollection* map) {
			m_pMap = map;
		}

		~ValueCollection() noexcept override = default;

		int Size() const override {
			return m_pMap->Size();
		}

		bool IsEmpty() const override {
			return m_pMap->IsEmpty();
		}

		CollectionType GetCollectionType() override { return CollectionType::ValueCollection; }

		TMapCollection* m_pMap;
	};

	struct ValueCollectionIterator : public Iterator<TValue, TAllocator>
	{
		using TValueIterator = Iterator<TValue, TAllocator>;

		ValueCollectionIterator(VoidOwner& owner, TMapCollectionIterator* iterator)
			: TValueIterator(owner) {
			m_pMapIterator = iterator;
		}

		~ValueCollectionIterator() noexcept override = default;

		bool HasPrevious() const override {
			return m_pMapIterator->HasPrevious();
		}

		bool HasNext() const override {
			return m_pMapIterator->HasNext();
		}

		TValue& Next() override {
			return m_pMapIterator->Next().Value;
		}

		TValue& Previous() override {
			return m_pMapIterator->Previous().Value;
		}

		TValue& Current() override {
			return m_pMapIterator->Current().Value;
		}

		bool IsBegin() const override {
			return m_pMapIterator->IsBegin();
		}

		bool IsEnd() const override {
			return m_pMapIterator->IsEnd();
		}

		TMapCollectionIterator* m_pMapIterator;
	};
};

template <typename TKey, typename TValue, typename TAllocator>
MapCollection<TKey, TValue, TAllocator>::~MapCollection() noexcept {}

NS_JC_END