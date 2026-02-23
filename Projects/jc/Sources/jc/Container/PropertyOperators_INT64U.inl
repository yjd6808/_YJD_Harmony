/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 10:25:34 PM
 * =====================
 *
 */

#include "PropertyStatics.h"
#include "PropertyOperatorDefinitionMacro.h"

NS_JC_BEGIN

SG_OPERATION_DEF_BEGIN(_u64, Store)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs = rhs,       LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs = (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs = (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs = (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u64, Move)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs = rhs,       LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs = (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs = (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs = (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u64, Plus)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs += rhs,       LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs += (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs += (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs += (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u64, Minus)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs -= rhs,       LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs -= (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs -= (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs -= (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u64, Multiply)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs *= rhs,       LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs *= (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs *= (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs *= (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

// _s32 / long long이 데이터 손실날 경우는 뭐가있을까?
SG_OPERATION_DEF_BEGIN(_u64, Divide)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs /= (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs /= (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs /= (_u64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_u64, Modulus)
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

SG_OPERATION_DEF_BEGIN(_u64, Equal)
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

SG_OPERATION_DEF_BEGIN(_u64, NotEqual)
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

SG_OPERATION_DEF_BEGIN(_u64, GreatorEqual)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = (RTy)lhs >= rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = (RTy)lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END


SG_OPERATION_DEF_BEGIN(_u64, Greator)
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

SG_OPERATION_DEF_BEGIN(_u64, LessEqual)
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

SG_OPERATION_DEF_BEGIN(_u64, Less)
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
void InitializePropertyOperatorInt64U()
{
	PropertyStatics::BinaryOperators__u64[PropertyBinaryOperatorType::Store]           = dbg_new PropertyValueBinaryOperator__u64_Store{};
	PropertyStatics::BinaryOperators__u64[PropertyBinaryOperatorType::Move]            = dbg_new PropertyValueBinaryOperator__u64_Move{};
	PropertyStatics::BinaryOperators__u64[PropertyBinaryOperatorType::Plus]            = dbg_new PropertyValueBinaryOperator__u64_Plus{};
	PropertyStatics::BinaryOperators__u64[PropertyBinaryOperatorType::Minus]           = dbg_new PropertyValueBinaryOperator__u64_Minus{};
	PropertyStatics::BinaryOperators__u64[PropertyBinaryOperatorType::Multiply]        = dbg_new PropertyValueBinaryOperator__u64_Multiply{};
	PropertyStatics::BinaryOperators__u64[PropertyBinaryOperatorType::Divide]          = dbg_new PropertyValueBinaryOperator__u64_Divide{};
	PropertyStatics::BinaryOperators__u64[PropertyBinaryOperatorType::Modulus]         = dbg_new PropertyValueBinaryOperator__u64_Modulus{};
	PropertyStatics::BinaryOperators__u64[PropertyBinaryOperatorType::Equal]           = dbg_new PropertyValueBinaryOperator__u64_Equal{};
	PropertyStatics::BinaryOperators__u64[PropertyBinaryOperatorType::NotEqual]        = dbg_new PropertyValueBinaryOperator__u64_NotEqual{};
	PropertyStatics::BinaryOperators__u64[PropertyBinaryOperatorType::GreatorEqual]    = dbg_new PropertyValueBinaryOperator__u64_GreatorEqual{};
	PropertyStatics::BinaryOperators__u64[PropertyBinaryOperatorType::Greator]         = dbg_new PropertyValueBinaryOperator__u64_Greator{};
	PropertyStatics::BinaryOperators__u64[PropertyBinaryOperatorType::LessEqual]       = dbg_new PropertyValueBinaryOperator__u64_LessEqual{};
	PropertyStatics::BinaryOperators__u64[PropertyBinaryOperatorType::Less]            = dbg_new PropertyValueBinaryOperator__u64_Less{};
}

NS_END