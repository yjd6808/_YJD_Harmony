/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/TypeCast.h>

NS_JC_BEGIN

template <typename TKey, typename TValue>
struct Pair
{
	TKey Key;
	TValue Value;
};

template <typename TKey, typename TValue>
constexpr Pair<NaturalType_t<TKey>, NaturalType_t<TValue>> MakePair(TKey&& key, TValue&& value) {
	return { Forward<TKey>(key), Forward<TValue>(value) };
}

NS_JC_END