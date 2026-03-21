/*
	작성자 : 윤정도
*/

#pragma once

#include "jc/Container/Iterator.h"

NS_JC_BEGIN

// 전방 선언
class CVoidOwner;

template <typename, typename>
class ListCollection;

template <typename, typename>
struct Pair;

template <typename TKey, typename TValue, typename TAllocator>
class JC_NOVTABLE MapCollectionIterator : public Iterator<Pair<TKey, TValue>, TAllocator>
{
	using TKeyValuePair = Pair<TKey, TValue>;
	using TIterator		= Iterator<TKeyValuePair, TAllocator>;

public:
	MapCollectionIterator(CVoidOwner& _owner)
		: TIterator(_owner)
	{
	}

	~MapCollectionIterator() noexcept override = 0;
};

template <typename TKey, typename TValue, typename TAllocator>
MapCollectionIterator<TKey, TValue, TAllocator>::~MapCollectionIterator() noexcept
{
}

NS_END
