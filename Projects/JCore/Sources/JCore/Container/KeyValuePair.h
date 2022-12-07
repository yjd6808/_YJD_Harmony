/*
	작성자 : 윤정도
*/

#pragma once

namespace JCore {

template <typename TKey, typename TValue>
struct KeyValuePair
{
	TKey Key;
	TValue Value;
};

template <typename TKey, typename TValue>
constexpr KeyValuePair<NaturalType_t<TKey>, NaturalType_t<TValue>> MakePair(TKey&& key, TValue&& value) {
	return { static_cast<TKey&&>(key), static_cast<TValue&&>(value) };
	// return { Forward<TKey>(key), Forward<TValue>(value) };	// 겨우 이거 하나땜에 헤더파일 포함시키긴 좀
}


} // namespace JCore