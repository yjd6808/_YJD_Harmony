/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 10:25:34 PM
 * =====================
 *
 */

#include "PropertyStatics.h"
#include "PropertyOperatorDefinitionMacro.h"

NS_JC_BEGIN

SG_OPERATION_DEF_BEGIN(_u16, Store)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs,		 LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u16, Move)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs,		 LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs = (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u16, Plus)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs += (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs += (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs += (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs += (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs += (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs += (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs += rhs,	 	  LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs += (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs += (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs += (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u16, Minus)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs -= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs -= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs -= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs -= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs -= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs -= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs -= rhs,	 	  LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs -= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs -= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs -= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u16, Multiply)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs *= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs *= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs *= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs *= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs *= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs *= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs *= rhs,	 	  LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs *= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs *= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs *= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

// _s32 / long long이 데이터 손실날 경우는 뭐가있을까?
SG_OPERATION_DEF_BEGIN(_u16, Divide)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs = (LTy)((RTy)lhs / rhs), LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs = (LTy)((RTy)lhs / rhs), LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs = (LTy)((RTy)lhs / rhs), LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs = (LTy)((RTy)lhs / rhs), LogLossDataPossible)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs /= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs /= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs /= (_u16)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u16, Modulus)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u16, Equal)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u16, NotEqual)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u16, GreatorEqual)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = (RTy)lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END


SG_OPERATION_DEF_BEGIN(_u16, Greator)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u16, LessEqual)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u16, Less)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END


//////////////////////////////////////////////////////////////////////////////////////////
void InitializePropertyOperatorInt16U()
{
	PropertyStatics::BinaryOperators__u16[PropertyBinaryOperatorType::Store]			= dbg_new PropertyValueBinaryOperator__u16_Store{};
	PropertyStatics::BinaryOperators__u16[PropertyBinaryOperatorType::Move]			= dbg_new PropertyValueBinaryOperator__u16_Move{};
	PropertyStatics::BinaryOperators__u16[PropertyBinaryOperatorType::Plus]			= dbg_new PropertyValueBinaryOperator__u16_Plus{};
	PropertyStatics::BinaryOperators__u16[PropertyBinaryOperatorType::Minus]			= dbg_new PropertyValueBinaryOperator__u16_Minus{};
	PropertyStatics::BinaryOperators__u16[PropertyBinaryOperatorType::Multiply]		= dbg_new PropertyValueBinaryOperator__u16_Multiply{};
	PropertyStatics::BinaryOperators__u16[PropertyBinaryOperatorType::Divide]			= dbg_new PropertyValueBinaryOperator__u16_Divide{};
	PropertyStatics::BinaryOperators__u16[PropertyBinaryOperatorType::Modulus]		= dbg_new PropertyValueBinaryOperator__u16_Modulus{};
	PropertyStatics::BinaryOperators__u16[PropertyBinaryOperatorType::Equal]			= dbg_new PropertyValueBinaryOperator__u16_Equal{};
	PropertyStatics::BinaryOperators__u16[PropertyBinaryOperatorType::NotEqual]		= dbg_new PropertyValueBinaryOperator__u16_NotEqual{};
	PropertyStatics::BinaryOperators__u16[PropertyBinaryOperatorType::GreatorEqual]	= dbg_new PropertyValueBinaryOperator__u16_GreatorEqual{};
	PropertyStatics::BinaryOperators__u16[PropertyBinaryOperatorType::Greator]		= dbg_new PropertyValueBinaryOperator__u16_Greator{};
	PropertyStatics::BinaryOperators__u16[PropertyBinaryOperatorType::LessEqual]		= dbg_new PropertyValueBinaryOperator__u16_LessEqual{};
	PropertyStatics::BinaryOperators__u16[PropertyBinaryOperatorType::Less]			= dbg_new PropertyValueBinaryOperator__u16_Less{};
}

NS_END