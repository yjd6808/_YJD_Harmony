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
struct Pair;

template <typename TKey, typename TValue>
class JCORE_NOVTABLE MapCollectionIterator : public Iterator<Pair<TKey, TValue>>
{
	using TKeyValuePair = Pair<TKey, TValue>;
	using TIterator		= Iterator<TKeyValuePair>;
public:
	MapCollectionIterator(VoidOwner& owner) : TIterator(owner) {}
	~MapCollectionIterator() noexcept override = 0;
};

template <typename TKey, typename TValue>
MapCollectionIterator<TKey, TValue>::~MapCollectionIterator() noexcept {

}

} // namespace JCore