/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 10:25:34 PM
 * =====================
 *
 */

#include "PropertyStatics.h"
#include "PropertyOperatorDefinitionMacro.h"

NS_JC_BEGIN

SG_OPERATION_DEF_BEGIN(_u32, Store)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs = (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs = (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs = rhs,         LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs = (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs = (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs = (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32, Move)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs = (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs = (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs = rhs,         LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs = (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs = (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs = (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32, Plus)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs += (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs += (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs += rhs,         LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs += (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs += (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs += (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32, Minus)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs -= (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs -= (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs -= rhs,         LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs -= (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs -= (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs -= (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32, Multiply)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs *= (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs *= (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs *= rhs,         LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs *= (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs *= (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs *= (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

// _s32 / long long이 데이터 손실날 경우는 뭐가있을까?
SG_OPERATION_DEF_BEGIN(_u32, Divide)
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
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs /= (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs /= (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs /= (_u32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u32, Modulus)
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

SG_OPERATION_DEF_BEGIN(_u32, Equal)
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

SG_OPERATION_DEF_BEGIN(_u32, NotEqual)
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

SG_OPERATION_DEF_BEGIN(_u32, GreatorEqual)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END


SG_OPERATION_DEF_BEGIN(_u32, Greator)
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

SG_OPERATION_DEF_BEGIN(_u32, LessEqual)
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

SG_OPERATION_DEF_BEGIN(_u32, Less)
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
void InitializePropertyOperatorInt32U()
{
	PropertyStatics::BinaryOperators__u32[PropertyBinaryOperatorType::Store]           = dbg_new PropertyValueBinaryOperator__u32_Store{};
	PropertyStatics::BinaryOperators__u32[PropertyBinaryOperatorType::Move]            = dbg_new PropertyValueBinaryOperator__u32_Move{};
	PropertyStatics::BinaryOperators__u32[PropertyBinaryOperatorType::Plus]            = dbg_new PropertyValueBinaryOperator__u32_Plus{};
	PropertyStatics::BinaryOperators__u32[PropertyBinaryOperatorType::Minus]           = dbg_new PropertyValueBinaryOperator__u32_Minus{};
	PropertyStatics::BinaryOperators__u32[PropertyBinaryOperatorType::Multiply]        = dbg_new PropertyValueBinaryOperator__u32_Multiply{};
	PropertyStatics::BinaryOperators__u32[PropertyBinaryOperatorType::Divide]          = dbg_new PropertyValueBinaryOperator__u32_Divide{};
	PropertyStatics::BinaryOperators__u32[PropertyBinaryOperatorType::Modulus]         = dbg_new PropertyValueBinaryOperator__u32_Modulus{};
	PropertyStatics::BinaryOperators__u32[PropertyBinaryOperatorType::Equal]           = dbg_new PropertyValueBinaryOperator__u32_Equal{};
	PropertyStatics::BinaryOperators__u32[PropertyBinaryOperatorType::NotEqual]        = dbg_new PropertyValueBinaryOperator__u32_NotEqual{};
	PropertyStatics::BinaryOperators__u32[PropertyBinaryOperatorType::GreatorEqual]    = dbg_new PropertyValueBinaryOperator__u32_GreatorEqual{};
	PropertyStatics::BinaryOperators__u32[PropertyBinaryOperatorType::Greator]         = dbg_new PropertyValueBinaryOperator__u32_Greator{};
	PropertyStatics::BinaryOperators__u32[PropertyBinaryOperatorType::LessEqual]       = dbg_new PropertyValueBinaryOperator__u32_LessEqual{};
	PropertyStatics::BinaryOperators__u32[PropertyBinaryOperatorType::Less]            = dbg_new PropertyValueBinaryOperator__u32_Less{};
}

NS_END