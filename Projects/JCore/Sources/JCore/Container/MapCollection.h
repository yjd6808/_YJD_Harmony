/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/KeyValuePair.h>
#include <JCore/Container/Collection.h>
#include <JCore/Container/MapCollectionIterator.h>

namespace JCore {

/*=====================================================================================
									맵 콜렉션
					      트리 맵, 해쉬 맵 공통 인터페이스 정의
=====================================================================================*/

template <typename TKey, typename TValue>
class JCORE_NOVTABLE MapCollection : public Collection<KeyValuePair<TKey, TValue>>
{
	using TKeyValuePair			 = KeyValuePair<TKey, TValue>;
	using TCollection			 = Collection<TKeyValuePair>;
	using TMapCollection		 = MapCollection<TKey, TValue>;
	using TMapCollectionIterator = MapCollectionIterator<TKey, TValue>;


public:
	MapCollection(ContainerType containerType) : TCollection(CollectionType::Map, containerType) {}
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
	virtual KeyCollection& Keys() = 0;
	virtual ValueCollection& Values() = 0;

	struct KeyCollection : public Collection<TKey>
	{
		using TkeyCollection = Collection<TKey>;

		KeyCollection(TMapCollection* map, ContainerType containerType)
			: TkeyCollection(CollectionType::KeyCollection, containerType) 
		{
			m_pMap = map;
		}

		~KeyCollection() noexcept override = default;

		int Size() const override {
			return m_pMap->Size();
		}

		bool IsEmpty() const override {
			return m_pMap->IsEmpty();
		}

		TMapCollection* m_pMap;
	};

	struct KeyCollectionIterator : public Iterator<TKey>
	{
		using TKeyIterator = Iterator<TKey>;

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

		bool IsBegin() const override {
			return m_pMapIterator->IsBegin();
		}

		bool IsEnd() const override {
			return m_pMapIterator->IsEnd();
		}

		TMapCollectionIterator* m_pMapIterator;
	};

	struct ValueCollection : public Collection<TValue>
	{
		using TValueCollection = Collection<TValue>;

		ValueCollection(TMapCollection* map, ContainerType containerType)
			: TValueCollection(CollectionType::ValueCollection, containerType) {
			m_pMap = map;
		}

		~ValueCollection() noexcept override = default;

		int Size() const override {
			return m_pMap->Size();
		}

		bool IsEmpty() const override {
			return m_pMap->IsEmpty();
		}

		TMapCollection* m_pMap;
	};

	struct ValueCollectionIterator : public Iterator<TValue>
	{
		using TValueIterator = Iterator<TValue>;

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

		bool IsBegin() const override {
			return m_pMapIterator->IsBegin();
		}

		bool IsEnd() const override {
			return m_pMapIterator->IsEnd();
		}

		TMapCollectionIterator* m_pMapIterator;
	};
};

template <typename TKey, typename TValue>
MapCollection<TKey, TValue>::~MapCollection() noexcept {}

} // namespace JCore