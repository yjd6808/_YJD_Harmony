/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/KeyValuePair.h>
#include <JCore/Container/Collection.h>

namespace JCore {

/*=====================================================================================
									맵 콜렉션
					      트리 맵, 해쉬 맵 공통 인터페이스 정의
=====================================================================================*/

template <typename TKey, typename TValue>
class MapCollection : public Collection<KeyValuePair<TKey, TValue>>
{
	using TKeyValuePair		= typename KeyValuePair<TKey, TValue>;
	using TCollection		= typename Collection<TKeyValuePair>;
	using TMapCollection	= typename MapCollection<TKey, TValue>;
public:
	MapCollection(ContainerType containerType) : TCollection(CollectionType::Map, containerType) {}
	virtual ~MapCollection() noexcept = 0;
public:
	virtual void Insert(const TKeyValuePair& pair) = 0;
	virtual void Insert(TKeyValuePair&& pair) = 0;
	virtual void Clear() = 0;
	virtual bool Find(const TKey& key) const = 0;
	virtual TValue& Get(const TKey& key) const = 0;
	virtual bool Remove(const TKey& key) = 0;
};

template <typename TKey, typename TValue>
MapCollection<TKey, TValue>::~MapCollection() noexcept {}

} // namespace JCore