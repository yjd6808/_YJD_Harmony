/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 10:25:34 PM
 * =====================
 *
 */

#include "PropertyStatics.h"
#include "PropertyOperatorDefinitionMacro.h"

NS_JC_BEGIN

SG_OPERATION_DEF_BEGIN(_s64, Store)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs = rhs,        LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs = (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs = (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs = (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_s64, Move)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs = rhs,        LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs = (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs = (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs = (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_s64, Plus)
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
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs += (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs += (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs += (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_s64, Minus)
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
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs -= (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs -= (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs -= (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_s64, Multiply)
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
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs *= (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs *= (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs *= (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END



SG_OPERATION_DEF_BEGIN(_s64, Divide)
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
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs /= (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs /= (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs /= (_s64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_s64, Modulus)
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

SG_OPERATION_DEF_BEGIN(_s64, Equal)
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

SG_OPERATION_DEF_BEGIN(_s64, NotEqual)
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

SG_OPERATION_DEF_BEGIN(_s64, GreatorEqual)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = (RTy)lhs >= rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = (RTy)lhs >= rhs)
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


SG_OPERATION_DEF_BEGIN(_s64, Greator)
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

SG_OPERATION_DEF_BEGIN(_s64, LessEqual)
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

SG_OPERATION_DEF_BEGIN(_s64, Less)
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


void InitializePropertyOperatorInt64()
{
	PropertyStatics::BinaryOperators__s64[PropertyBinaryOperatorType::Store]            = dbg_new PropertyValueBinaryOperator__s64_Store{};
	PropertyStatics::BinaryOperators__s64[PropertyBinaryOperatorType::Move]             = dbg_new PropertyValueBinaryOperator__s64_Move{};
	PropertyStatics::BinaryOperators__s64[PropertyBinaryOperatorType::Plus]             = dbg_new PropertyValueBinaryOperator__s64_Plus{};
	PropertyStatics::BinaryOperators__s64[PropertyBinaryOperatorType::Minus]            = dbg_new PropertyValueBinaryOperator__s64_Minus{};
	PropertyStatics::BinaryOperators__s64[PropertyBinaryOperatorType::Multiply]         = dbg_new PropertyValueBinaryOperator__s64_Multiply{};
	PropertyStatics::BinaryOperators__s64[PropertyBinaryOperatorType::Divide]           = dbg_new PropertyValueBinaryOperator__s64_Divide{};
	PropertyStatics::BinaryOperators__s64[PropertyBinaryOperatorType::Modulus]          = dbg_new PropertyValueBinaryOperator__s64_Modulus{};
	PropertyStatics::BinaryOperators__s64[PropertyBinaryOperatorType::Equal]            = dbg_new PropertyValueBinaryOperator__s64_Equal{};
	PropertyStatics::BinaryOperators__s64[PropertyBinaryOperatorType::NotEqual]         = dbg_new PropertyValueBinaryOperator__s64_NotEqual{};
	PropertyStatics::BinaryOperators__s64[PropertyBinaryOperatorType::GreatorEqual]     = dbg_new PropertyValueBinaryOperator__s64_GreatorEqual{};
	PropertyStatics::BinaryOperators__s64[PropertyBinaryOperatorType::Greator]          = dbg_new PropertyValueBinaryOperator__s64_Greator{};
	PropertyStatics::BinaryOperators__s64[PropertyBinaryOperatorType::LessEqual]        = dbg_new PropertyValueBinaryOperator__s64_LessEqual{};
	PropertyStatics::BinaryOperators__s64[PropertyBinaryOperatorType::Less]             = dbg_new PropertyValueBinaryOperator__s64_Less{};

}

NS_END