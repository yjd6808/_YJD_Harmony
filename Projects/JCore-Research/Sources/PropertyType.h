/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 6:01:55 PM
 * =====================
 *
 */


#pragma once

#include "header.h"

JCORE_SENUM_BEGIN(PropertyType)
Int8,
Int8U,
Int16,
Int16U,
Int,
Int32U,
Int32L,
Int32UL,
Int64,
Int64U,
Float,
Double,
LDouble,
String,
CharPtr,
Unknown,
Max
JCORE_SENUM_MIDDLE(PropertyType)
static constexpr const char* Name[Max]{
	"Int8",
	"Int8U",
	"Int16",
	"Int16U",
	"Int",
	"Int32U",
	"Int32L",
	"Int32UL",
	"Int64",
	"Int64U",
	"Float",
	"Double",
	"LDouble",
	"String",
	"CharPtr",
	"Unknown",
};

static constexpr bool IsIntegerType[Max]{
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	false,
	false,
	false,
	false,
	false,
	false
};

static constexpr bool IsFloatType[Max]{
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	true,
	true,
	true,
	false,
	false,
	false
};

static constexpr bool IsNumericType[Max]{
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	false,
	false,
	false
};

static constexpr bool IsStringType[Max]{
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	true,
	true,
	false
};

// CharPtr, Unknwon 제외
static constexpr bool CanBeLeftOperand[Max]{
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	false,
	false
};

// Unknwon 제외
static constexpr bool CanBeRightOperand[Max]{
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	false
};

// CharPtr, Unknwon 제외
static constexpr bool CanConstruct[Max]{
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	false,
	false
};

// CharPtr만
static constexpr bool IsPtrType[Max]{
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	true,
	false
};

static constexpr bool IsConvertiable(PropertyType_t lhs, PropertyType_t rhs) {
	if (IsNumericType[lhs] && IsNumericType[rhs]) {
		return true;
	}
	if (IsStringType[lhs] && IsStringType[rhs]) {
		return true;
	}
	return false;
}

JCORE_SENUM_MIDDLE_END(PropertyType)


JCORE_SENUM_BEGIN(PropertyUnaryOperatorType)
PrefixIncrement,
PrefixDecrement,
PostfixIncrement,
PostfixDecrement,
Max
JCORE_SENUM_MIDDLE(PropertyUnaryOperatorType)
static constexpr const char* Name[Max] = {
	"++",
	"--",
	"++(int)",
	"--(int)",
};

JCORE_SENUM_MIDDLE_END(PropertyUnaryOperatorType)


JCORE_SENUM_BEGIN(PropertyBinaryOperatorType)
Store,
Move,
Plus,
Minus,
Multiply,
Divide,
Modulus,
Equal,
NotEqual,
GreatorEqual,
Greator,
LessEqual,
Less,
Unknwon,
Max
JCORE_SENUM_MIDDLE(PropertyBinaryOperatorType)

static constexpr const char* Name[Max] = {
	"=",
	"=&&",
	"+",
	"-",
	"*",
	"/",
	"%=",
	"==",
	"!=",
	">=",
	">",
	"<=",
	"<",
	"Unknown"
};

JCORE_SENUM_MIDDLE_END(PropertyBinaryOperatorType)
