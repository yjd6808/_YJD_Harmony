/*
	작성자 : 윤정도
	최대 최소 수치를 표현
*/

#pragma once

#include "jc/Type.h"
#include "jc/Namespace.h"

#pragma warning (push)
#pragma warning (disable : 4307)	// '+': signed integral constant overflow
#pragma warning (disable : 4309)	// 'initializing': truncation of constant value
#pragma warning (disable : 26450)	// Arithmetic overflow : '+' operation causes overflow at compile time.Use a wider type to store the operands -> MinInt32_v 이런거처럼 내가 고의로 오버플로 일으킨 것 땜에



NS_JC_BEGIN
NS_DETAIL_BEGIN
	enum class ForSizeCheck
	{
	};

NS_END

constexpr char MaxChar_v = (1 << 7) - 1;
constexpr char MinChar_v = (1 << 7);
constexpr char MaxInt8_v = (1 << 7) - 1;
constexpr char MinInt8_v = (1 << 7);
constexpr _u8 MaxByte_v = (1 << 8) - 1;
constexpr _u8 MaxInt8U_v = (1 << 8) - 1;
constexpr _s16 MaxInt16_v = (1 << 15) - 1;
constexpr _s16 MinInt16_v = (1 << 15);
constexpr _s16 MaxShort_v = (1 << 15) - 1;
constexpr _s16 MinShort_v = (1 << 15);
constexpr _u16 MaxInt16U_v = (1 << 16) - 1;
constexpr int MaxInt32_v = 0x7fffffff;
constexpr int MinInt32_v = -2147483648;
constexpr int MaxInt_v = 0x7fffffff;
constexpr int MinInt_v = MinInt32_v;
constexpr _u32 MaxInt32U_v = 0xffffffffU;
constexpr _s64 MaxInt64_v = 0x7fffffffffffffffLL;
constexpr _s64 MinInt64_v = LLONG_MIN;
constexpr _u64 MaxInt64U_v = 0xffffffffffffffffLLU;
constexpr int BytePerBit_v = 8;
constexpr int EnumSize_v = sizeof(detail::ForSizeCheck);

template <typename T>
constexpr int BitCount()
{
	return sizeof(T) * BytePerBit_v;
}

template <typename T>
constexpr int BitCount(T&& _val)
{
	return sizeof(_val) * BytePerBit_v;
}

NS_END

#pragma warning (pop)
