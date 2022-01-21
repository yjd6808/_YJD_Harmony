#pragma once

/*
	�ۼ��� : ������
	������ Ʃ��
*/

#include <JCore/TypeTraits.h>

namespace JCore {

template <typename... Args>
struct Tuple;

template <>
struct Tuple<>;

// 4�� �̻��� �Ⱦ�����..?
template <typename T1>
struct Tuple<T1>
{
	T1 item1;
};

template <typename T1, typename T2>
struct Tuple<T1, T2>
{
	T1 item1;
	T2 item2;
};

template <typename T1, typename T2, typename T3>
struct Tuple<T1, T2, T3>
{
	T1 item1;
	T2 item2;
	T3 item3;
};

template <typename T1, typename T2, typename T3, typename T4>
struct Tuple<T1, T2, T3, T4>
{
	T1 item1;
	T2 item2;
	T3 item3;
	T4 item4;
};


template <typename... Args>
auto MakeTuple(Args&&... args) {
	return Tuple<RemoveReference_t<Args>...> { Forward<Args>(args)... };
}


} // namespace JCore