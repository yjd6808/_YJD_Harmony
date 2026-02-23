/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 6:01:55 PM
 * =====================
 *
 */


#pragma once

#include <jc/Namespace.h>
#include <jc/Define.h>
#include <jc/Declspec.h>
#include <jc/Macro.h>
#include <jc/Type.h>

NS_JC_BEGIN

JC_SENUM_BEGIN(PropertyType)
Bool,
_s8,
_u8,
_s16,
_u16,
_s32,
_u32,
_s32l,
_u32l,
_s64,
_u64,
_f32,
_f64,
_f64l,
String,
CharPtr,
Unknown,
Max
JC_SENUM_MIDDLE(PropertyType)
static constexpr const char* NAME[Max]
{
	"Bool",
	"_s8",
	"_u8",
	"_s16",
	"_u16",
	"_s32",
	"_u32",
	"_s32l",
	"_u32l",
	"_s64",
	"_u64",
	"_f32",
	"_f64",
	"_f64l",
	"String",
	"CharPtr",
	"Unknown",
};

static constexpr bool IS_INTEGER_TYPE[Max]
{
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
	false
};

static constexpr bool IS_FLOAT_TYPE[Max]
{
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
	false
};

static constexpr bool IS_NUMERIC_TYPE[Max]
{
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
	false
};

static constexpr bool IS_STRING_TYPE[Max]
{
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
	true,
	false
};

// CharPtr, Unknwon 제외
static constexpr bool CAN_BE_LEFT_OPERAND[Max]
{
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
	false,
	false
};

// Unknwon 제외
static constexpr bool CAN_BE_RIGHT_OPERAND[Max]
{
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
	true,
	false
};

// CharPtr, Unknwon 제외
static constexpr bool CAN_CONSTRUCT[Max]
{
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
	false,
	false
};

// CharPtr만
static constexpr bool IS_PTR_TYPE[Max]
{
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
	false,
	true,
	false
};

static constexpr bool IsConvertiable(PropertyType_t _lhs, PropertyType_t _rhs)
{
	if (IS_NUMERIC_TYPE[_lhs] && IS_NUMERIC_TYPE[_rhs])
	{
		return true;
	}

	if (IS_STRING_TYPE[_lhs] && IS_STRING_TYPE[_rhs])
	{
		return true;
	}

	return false;
}

JC_SENUM_MIDDLE_END(PropertyType)


JC_SENUM_BEGIN(PropertyUnaryOperatorType)
PrefixIncrement,
PrefixDecrement,
PostfixIncrement,
PostfixDecrement,
Max
JC_SENUM_MIDDLE(PropertyUnaryOperatorType)
static constexpr const char* NAME[Max] =
{
	"++",
	"--",
	"++(int)",
	"--(int)",
};

JC_SENUM_MIDDLE_END(PropertyUnaryOperatorType)


JC_SENUM_BEGIN(PropertyBinaryOperatorType)
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
JC_SENUM_MIDDLE(PropertyBinaryOperatorType)

static constexpr const char* NAME[Max] =
{
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

JC_SENUM_MIDDLE_END(PropertyBinaryOperatorType)


NS_END
