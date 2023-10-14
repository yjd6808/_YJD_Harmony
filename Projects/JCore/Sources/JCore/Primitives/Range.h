/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 11:05:30 AM
 * =====================
 *
 */


#pragma once


#include <JCore/Namespace.h>
#include <JCore/Type.h>

NS_JC_BEGIN

template <typename TNumeric>
struct Range
{
	TNumeric Min;	// Inclusive
	TNumeric Max;	// Exclusive

	constexpr bool Contain(TNumeric val) const {
		return val >= Min && val < Max;
	}

	constexpr TNumeric Average() const {
		return (Min + Max) / TNumeric(2);
	}
};


using RangeInt64 = Range<Int64>;
using RangeInt64U = Range<Int64U>;
using RangeInt = Range<Int>;
using RangeInt32 = Range<Int32>;
using RangeInt32U = Range<Int32U>;
using RangeInt32L = Range<Int32L>;
using RangeInt32UL = Range<Int32UL>;
using RangeInt16 = Range<Int16>;
using RangeInt16U = Range<Int16U>;
using RangeInt8 = Range<Int8>;
using RangeInt8U = Range<Int8U>;
using RangeChar = Range<Char>;
using RangeDouble = Range<Double>;
using RangeLDouble = Range<LDouble>;
using RangeByte = Range<Byte>;
using RangeWideChar = Range<WideChar>;

NS_JC_END