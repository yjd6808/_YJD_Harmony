/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 10:25:34 PM
 * =====================
 *
 */

#include "PropertyStatics.h"
#include "PropertyOperatorDefinitionMacro.h"

NS_JC_BEGIN

SG_OPERATION_DEF_BEGIN(_u32l, Store)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs = (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs = (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs = rhs,          LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs = (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs = (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs = (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32l, Move)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs = (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs = (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs = rhs,          LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs = (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs = (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs = (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32l, Plus)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs += (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs += (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs += rhs,         LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs += (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs += (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs += (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32l, Minus)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs -= (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs -= (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs -= rhs,         LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs -= (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs -= (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs -= (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32l, Multiply)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs *= (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs *= (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs *= rhs,         LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs *= (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs *= (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs *= (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

// _s32 / long long이 데이터 손실날 경우는 뭐가있을까?
SG_OPERATION_DEF_BEGIN(_u32l, Divide)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs = (LTy)((RTy)lhs / rhs), LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs = (LTy)((RTy)lhs / rhs), LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs /= (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs /= (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs /= (_u32l)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32l, Modulus)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32l, Equal)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32l, NotEqual)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32l, GreatorEqual)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(PropertyStatics::ComparisonResult = (RTy)lhs >= rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = (RTy)lhs >= rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = (RTy)lhs >= rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = (RTy)lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = (RTy)lhs >= rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32l, Greator)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > (LTy)rhs)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32l, LessEqual)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= (LTy)rhs)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32l, Less)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < (LTy)rhs)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

//////////////////////////////////////////////////////////////////////////////////////////
void InitializePropertyOperatorInt32UL()
{
	PropertyStatics::BinaryOperators__u32l[PropertyBinaryOperatorType::Store]          = dbg_new PropertyValueBinaryOperator__u32l_Store{};
	PropertyStatics::BinaryOperators__u32l[PropertyBinaryOperatorType::Move]           = dbg_new PropertyValueBinaryOperator__u32l_Move{};
	PropertyStatics::BinaryOperators__u32l[PropertyBinaryOperatorType::Plus]           = dbg_new PropertyValueBinaryOperator__u32l_Plus{};
	PropertyStatics::BinaryOperators__u32l[PropertyBinaryOperatorType::Minus]          = dbg_new PropertyValueBinaryOperator__u32l_Minus{};
	PropertyStatics::BinaryOperators__u32l[PropertyBinaryOperatorType::Multiply]       = dbg_new PropertyValueBinaryOperator__u32l_Multiply{};
	PropertyStatics::BinaryOperators__u32l[PropertyBinaryOperatorType::Divide]         = dbg_new PropertyValueBinaryOperator__u32l_Divide{};
	PropertyStatics::BinaryOperators__u32l[PropertyBinaryOperatorType::Modulus]        = dbg_new PropertyValueBinaryOperator__u32l_Modulus{};
	PropertyStatics::BinaryOperators__u32l[PropertyBinaryOperatorType::Equal]          = dbg_new PropertyValueBinaryOperator__u32l_Equal{};
	PropertyStatics::BinaryOperators__u32l[PropertyBinaryOperatorType::NotEqual]       = dbg_new PropertyValueBinaryOperator__u32l_NotEqual{};
	PropertyStatics::BinaryOperators__u32l[PropertyBinaryOperatorType::GreatorEqual]   = dbg_new PropertyValueBinaryOperator__u32l_GreatorEqual{};
	PropertyStatics::BinaryOperators__u32l[PropertyBinaryOperatorType::Greator]        = dbg_new PropertyValueBinaryOperator__u32l_Greator{};
	PropertyStatics::BinaryOperators__u32l[PropertyBinaryOperatorType::LessEqual]      = dbg_new PropertyValueBinaryOperator__u32l_LessEqual{};
	PropertyStatics::BinaryOperators__u32l[PropertyBinaryOperatorType::Less]           = dbg_new PropertyValueBinaryOperator__u32l_Less{};
}

NS_END