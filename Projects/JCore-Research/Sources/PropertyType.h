/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 6:01:55 PM
 * =====================
 *
 */


#pragma once

#include "header.h"

JCORE_SENUM_BEGIN(PropertyArgumentType)
Int64		,
Int64U		,
Int			,
Int32U		,
Int32L		,
Int32UL		,
Int16		,
Int16U		,
Int8		,
Int8U		,
WideChar	,
Float		,
Double		,
LDouble		,
CharPtr		,
CharArray	,
String		,
Unknown		,
Max
JCORE_SENUM_MIDDLE(PropertyArgumentType)
static constexpr const char* Name[Max]{
	"Int64"		,
	"Int64U"	,
	"Int"		,
	"Int32U"	,
	"Int32L"	,
	"Int32UL"	,
	"Int16"		,
	"Int16U"	,
	"Int8"		,
	"Int8U"		,
	"WideChar"	,
	"Float"		,
	"Double"	,
	"LDouble"	,
	"CharPtr"	,
	"CharArray"	,
	"String"	,
	"Unknown"
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
	true,
	false,
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
	false,
	true,
	true,
	true,
	false,
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
	true,
	false,
	false,
	false,
	false
};


JCORE_SENUM_MIDDLE_END(PropertyArgumentType)

JCORE_SENUM_BEGIN(PropertyType)
Int,
Int64,
Float,
Double,
String,
Unknown,
Max
JCORE_SENUM_MIDDLE(PropertyType)

static constexpr PropertyArgumentType_t ToArgumentType[Max]{
	PropertyArgumentType::Int,
	PropertyArgumentType::Int64,
	PropertyArgumentType::Float,
	PropertyArgumentType::Double,
	PropertyArgumentType::String,
	PropertyArgumentType::Unknown
};

static constexpr const char* Name[Max]{
	"Int"		,
	"Int64"		,
	"Float"		,
	"Double"	,
	"String"	,
	"Unknown"
};

static constexpr bool IsIntegerType[Max]{
	true,
	true,
	false,
	false,
	false,
	false
};

static constexpr bool IsFloatType[Max]{
	false,
	false,
	true,
	true,
	false,
	false
};

static constexpr bool IsNumericType[Max]{
	true,
	true,
	true,
	true,
	false,
	false
};


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
