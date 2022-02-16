/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/Iterator.h>

namespace JCore {

//전방 선언
class VoidOwner;	

template <typename>	
class ListCollection;

template <typename, typename> 
struct KeyValuePair;

template <typename TKey, typename TValue>
class MapCollectionIterator : public Iterator<KeyValuePair<TKey, TValue>>
{
	using TKeyValuePair = typename KeyValuePair<TKey, TValue>;
	using TIterator		= typename Iterator<TKeyValuePair>;
public:
	MapCollectionIterator(VoidOwner& owner) : TIterator(owner) {}
	virtual ~MapCollectionIterator() noexcept = 0;
public:
	//virtual TKey& NextKey() = 0;
	//virtual TValue& NextValue() = 0;

	//virtual TKey& PreviousKey() = 0;
	//virtual TValue& PreviousValue() = 0;
};

template <typename TKey, typename TValue>
MapCollectionIterator<TKey, TValue>::~MapCollectionIterator() noexcept {

}

} // namespace JCore