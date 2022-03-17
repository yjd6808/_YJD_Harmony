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
class MapCollection : public Collection<KeyValuePair<TKey, TValue>>
{
	using TKeyValuePair			 = typename KeyValuePair<TKey, TValue>;
	using TCollection			 = typename Collection<TKeyValuePair>;
	using TMapCollection		 = typename MapCollection<TKey, TValue>;
	using TMapCollectionIterator = typename MapCollectionIterator<TKey, TValue>;


public:
	MapCollection(ContainerType containerType) : TCollection(CollectionType::Map, containerType) {}
	virtual ~MapCollection() noexcept = 0;

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
		using TkeyCollection = typename Collection<TKey>;

		KeyCollection(TMapCollection* map, ContainerType containerType)
			: TkeyCollection(CollectionType::KeyCollection, containerType) 
		{
			m_pMap = map;
		}
		virtual ~KeyCollection() noexcept = default;

		virtual int Size() const {
			return m_pMap->Size();
		}

		virtual bool IsEmpty() const {
			return m_pMap->IsEmpty();
		}

		TMapCollection* m_pMap;
	};

	struct KeyCollectionIterator : public Iterator<TKey>
	{
		using TKeyIterator = typename Iterator<TKey>;

		KeyCollectionIterator(VoidOwner& owner, TMapCollectionIterator* iterator) 
			: TKeyIterator(owner)
		{
			m_pMapIterator = iterator;
		}
		virtual ~KeyCollectionIterator() noexcept = default;


		virtual bool HasPrevious() const {
			return m_pMapIterator->HasPrevious();
		}

		virtual bool HasNext() const {
			return m_pMapIterator->HasNext();
		}

		virtual TKey& Next() {
			return m_pMapIterator->Next().Key;
		}

		virtual TKey& Previous() {
			return m_pMapIterator->Previous().Key;
		}

		virtual bool IsBegin() const {
			return m_pMapIterator->IsBegin();
		}

		virtual bool IsEnd() const {
			return m_pMapIterator->IsEnd();
		}

		TMapCollectionIterator* m_pMapIterator;
	};

	struct ValueCollection : public Collection<TValue>
	{
		using TValueCollection = typename Collection<TValue>;

		ValueCollection(TMapCollection* map, ContainerType containerType)
			: TValueCollection(CollectionType::ValueCollection, containerType) {
			m_pMap = map;
		}
		virtual ~ValueCollection() noexcept = default;

		virtual int Size() const {
			return m_pMap->Size();
		}

		virtual bool IsEmpty() const {
			return m_pMap->IsEmpty();
		}

		TMapCollection* m_pMap;
	};

	struct ValueCollectionIterator : public Iterator<TValue>
	{
		using TValueIterator = typename Iterator<TValue>;

		ValueCollectionIterator(VoidOwner& owner, TMapCollectionIterator* iterator)
			: TValueIterator(owner) {
			m_pMapIterator = iterator;
		}
		virtual ~ValueCollectionIterator() noexcept = default;

		virtual bool HasPrevious() const {
			return m_pMapIterator->HasPrevious();
		}

		virtual bool HasNext() const {
			return m_pMapIterator->HasNext();
		}

		virtual TValue& Next() {
			return m_pMapIterator->Next().Value;
		}

		virtual TValue& Previous() {
			return m_pMapIterator->Previous().Value;
		}

		virtual bool IsBegin() const {
			return m_pMapIterator->IsBegin();
		}

		virtual bool IsEnd() const {
			return m_pMapIterator->IsEnd();
		}

		TMapCollectionIterator* m_pMapIterator;
	};
};

template <typename TKey, typename TValue>
MapCollection<TKey, TValue>::~MapCollection() noexcept {}

} // namespace JCore