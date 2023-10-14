/*
 * 작성자: 윤정도
 * 생성일: 8/15/2023 5:53:07 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/Iterator.h>

NS_JC_BEGIN

class VoidOwner;
template <typename, typename> struct Pair;

template <typename TKey, typename TAllocator>
class JCORE_NOVTABLE SetCollectionIterator : public Iterator<TKey, TAllocator>
{
	using TIterator = Iterator<TKey, TAllocator>;
public:
	SetCollectionIterator(VoidOwner& owner) : TIterator(owner) {}
	~SetCollectionIterator() noexcept override = 0;
};

template <typename TKey, typename TAllocator>
SetCollectionIterator<TKey, TAllocator>::~SetCollectionIterator() noexcept {}

NS_JC_END