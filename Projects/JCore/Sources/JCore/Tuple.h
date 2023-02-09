/*
	작성자 : 윤정도
	간단한 튜플
*/

#pragma once

#include <JCore/TypeCast.h>

NS_JC_BEGIN

template <typename... Args>
struct Tuple;

template <>
struct Tuple<>;

// 4개 이상은 안쓰겠지..?
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


template <typename T1, typename T2, typename T3, typename T4, typename T5>
struct Tuple<T1, T2, T3, T4, T5>
{
	T1 item1;
	T2 item2;
	T3 item3;
	T4 item4;
	T5 item5;
};


template <typename... Args>
constexpr auto MakeTuple(Args&&... args) {
	return Tuple<RemoveReference_t<Args>...> { Forward<Args>(args)... };
}


NS_JC_END