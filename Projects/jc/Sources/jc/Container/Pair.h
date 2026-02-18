/*
	작성자 : 윤정도
*/

#pragma once

#include <jc/TypeCast.h>

NS_JC_BEGIN

template <typename TKey, typename TValue>
struct Pair
{
	TKey key_;
	TValue value_;
};

template <typename TKey, typename TValue>
constexpr Pair<NaturalType_t<TKey>, NaturalType_t<TValue>> MakePair(TKey&& _key, TValue&& _value)
{
	return { Forward<TKey>(_key), Forward<TValue>(_value) };
}

NS_END
