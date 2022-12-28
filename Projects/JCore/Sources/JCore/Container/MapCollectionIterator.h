/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/Iterator.h>

namespace JCore {

//전방 선언
class VoidOwner;	

template <typename, typename>	
class ListCollection;

template <typename, typename> 
struct Pair;

template <typename TKey, typename TValue, typename TAllocator>
class JCORE_NOVTABLE MapCollectionIterator : public Iterator<Pair<TKey, TValue>, TAllocator>
{
	using TKeyValuePair = Pair<TKey, TValue>;
	using TIterator		= Iterator<TKeyValuePair, TAllocator>;
public:
	MapCollectionIterator(VoidOwner& owner) : TIterator(owner) {}
	~MapCollectionIterator() noexcept override = 0;
};

template <typename TKey, typename TValue, typename TAllocator>
MapCollectionIterator<TKey, TValue, TAllocator>::~MapCollectionIterator() noexcept {

}

} // namespace JCore