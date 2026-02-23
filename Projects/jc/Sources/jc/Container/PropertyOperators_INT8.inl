/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 10:25:34 PM
 * =====================
 *
 */

#include "PropertyStatics.h"
#include "PropertyOperatorDefinitionMacro.h"

NS_JC_BEGIN

SG_OPERATION_DEF_BEGIN(_s8, Store)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs = rhs,       LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_s8, Move)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs = rhs,       LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs = (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_s8, Plus)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs += (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs += (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs += (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs += (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs += (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs += (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs += (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs += (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs += rhs,      LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs += (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs += (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs += (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_s8, Minus)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs -= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs -= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs -= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs -= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs -= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs -= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs -= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs -= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs -= rhs,      LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs -= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs -= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs -= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_s8, Multiply)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs *= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs *= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs *= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs *= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs *= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs *= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs *= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs *= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(lhs *= rhs,      LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs *= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs *= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs *= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_s8, Divide)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(lhs = (LTy)((RTy)lhs / rhs), LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(lhs = (LTy)((RTy)lhs / rhs), LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(lhs = (LTy)((RTy)lhs / rhs), LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(lhs = (LTy)((RTy)lhs / rhs), LogLossDataPossible)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(lhs /= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(lhs /= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(lhs /= (_s8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_s8, Modulus)
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

SG_OPERATION_DEF_BEGIN(_s8, Equal)
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

SG_OPERATION_DEF_BEGIN(_s8, NotEqual)
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

SG_OPERATION_DEF_BEGIN(_s8, GreatorEqual)
SG_OPERATION_OPERATE(const _s64&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const _u64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const _s32&)          SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const _u32&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const _s32l&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const _u32l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const _s16&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const _u16&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const bool&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)         SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const _u8&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const _f32&)        SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const _f64&)       SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const _f64l&)      SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= (LTy)rhs)
SG_OPERATION_OPERATE(const String&)       SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)               SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_s8, Greator)
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

SG_OPERATION_DEF_BEGIN(_s8, LessEqual)
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

SG_OPERATION_DEF_BEGIN(_s8, Less)
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
void InitializePropertyOperatorInt8()
{
	PropertyStatics::BinaryOperators__s8[PropertyBinaryOperatorType::Store]       = dbg_new PropertyValueBinaryOperator__s8_Store{};
	PropertyStatics::BinaryOperators__s8[PropertyBinaryOperatorType::Move]        = dbg_new PropertyValueBinaryOperator__s8_Move{};
	PropertyStatics::BinaryOperators__s8[PropertyBinaryOperatorType::Plus]        = dbg_new PropertyValueBinaryOperator__s8_Plus{};
	PropertyStatics::BinaryOperators__s8[PropertyBinaryOperatorType::Minus]       = dbg_new PropertyValueBinaryOperator__s8_Minus{};
	PropertyStatics::BinaryOperators__s8[PropertyBinaryOperatorType::Multiply]    = dbg_new PropertyValueBinaryOperator__s8_Multiply{};
	PropertyStatics::BinaryOperators__s8[PropertyBinaryOperatorType::Divide]      = dbg_new PropertyValueBinaryOperator__s8_Divide{};
	PropertyStatics::BinaryOperators__s8[PropertyBinaryOperatorType::Modulus]     = dbg_new PropertyValueBinaryOperator__s8_Modulus{};
	PropertyStatics::BinaryOperators__s8[PropertyBinaryOperatorType::Equal]       = dbg_new PropertyValueBinaryOperator__s8_Equal{};
	PropertyStatics::BinaryOperators__s8[PropertyBinaryOperatorType::NotEqual]    = dbg_new PropertyValueBinaryOperator__s8_NotEqual{};
	PropertyStatics::BinaryOperators__s8[PropertyBinaryOperatorType::GreatorEqual] = dbg_new PropertyValueBinaryOperator__s8_GreatorEqual{};
	PropertyStatics::BinaryOperators__s8[PropertyBinaryOperatorType::Greator]     = dbg_new PropertyValueBinaryOperator__s8_Greator{};
	PropertyStatics::BinaryOperators__s8[PropertyBinaryOperatorType::LessEqual]   = dbg_new PropertyValueBinaryOperator__s8_LessEqual{};
	PropertyStatics::BinaryOperators__s8[PropertyBinaryOperatorType::Less]        = dbg_new PropertyValueBinaryOperator__s8_Less{};
}

NS_END