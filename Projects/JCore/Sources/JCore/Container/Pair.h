/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/TypeCast.h>

namespace JCore {

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

} // namespace JCore