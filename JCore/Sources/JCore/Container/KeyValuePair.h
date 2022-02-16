/*
	�ۼ��� : ������
*/

#pragma once

namespace JCore {

template <typename TKey, typename TValue>
struct KeyValuePair
{
	using TKeyValuePair = typename KeyValuePair<TKey, TValue>;

	TKey Key;
	TValue Value;
};

template <typename TKey, typename TValue>
constexpr KeyValuePair<NaturalType_t<TKey>, NaturalType_t<TValue>> MakePair(TKey&& key, TValue&& value) {
	return { static_cast<TKey&&>(key), static_cast<TValue&&>(value) };
	// return { Forward<TKey>(key), Forward<TValue>(value) };	// �ܿ� �̰� �ϳ����� ������� ���Խ�Ű�� ��
}


} // namespace JCore