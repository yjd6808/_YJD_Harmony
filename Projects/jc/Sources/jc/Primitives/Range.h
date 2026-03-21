/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 11:05:30 AM
 * =====================
 *
 */


#pragma once


#include "jc/Namespace.h"
#include "jc/Type.h"

NS_JC_BEGIN

template <typename TNumeric>
struct Range
{
	TNumeric Min;	// Inclusive
	TNumeric Max;	// Exclusive

	constexpr bool Contain(TNumeric _val) const {
		return _val >= Min && _val < Max;
	}

	constexpr TNumeric Average() const {
		return (Min + Max) / TNumeric(2);
	}
};


using RangeInt64 = Range<_s64>;
using RangeInt64U = Range<_u64>;
using RangeInt = Range<_s32>;
using RangeInt32 = Range<_s32>;
using RangeInt32U = Range<_u32>;
using RangeInt32L = Range<_s32l>;
using RangeInt32UL = Range<_u32l>;
using RangeInt16 = Range<_s16>;
using RangeInt16U = Range<_u16>;
using RangeInt8 = Range<_s8>;
using RangeInt8U = Range<_u8>;
using RangeChar = Range<_s8>;
using RangeDouble = Range<_f64>;
using RangeLDouble = Range<_f64l>;
using RangeByte = Range<_u8>;
using RangeWideChar = Range<_s16c>;

NS_END