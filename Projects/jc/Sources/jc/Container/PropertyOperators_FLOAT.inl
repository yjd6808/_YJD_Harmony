/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 10:25:34 PM
 * =====================
 *
 */

#include "PropertyStatics.h"
#include "PropertyOperatorDefinitionMacro.h"

NS_JC_BEGIN

SG_OPERATION_DEF_BEGIN(_f32, Store)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f32, Move)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs = (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f32, Plus)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs += rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs += (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs += (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f32, Minus)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs -= rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs -= (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs -= (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f32, Multiply)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs *= rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs *= (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs *= (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f32, Divide)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs /= (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs /= (_f32)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f32, Modulus)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f32(rhs)), LogLossDataPossible)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f32(rhs)), LogLossDataPossible)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f32(rhs)),)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f32(rhs)),)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f32(rhs)),)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs = fmod(lhs, _f32(rhs)),)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f32(rhs)),)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f32(rhs)),)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f32(rhs)), LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f32(rhs)),)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f32(rhs)),)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f32(rhs)), LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f32(rhs)), LogLossDataPossible)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs = fmod(lhs, _f32(rhs)), LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f32, Equal)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f32, NotEqual)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f32, GreatorEqual)
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


SG_OPERATION_DEF_BEGIN(_f32, Greator)
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

SG_OPERATION_DEF_BEGIN(_f32, LessEqual)
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

SG_OPERATION_DEF_BEGIN(_f32, Less)
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
void InitializePropertyOperatorFloat()
{
	PropertyStatics::BinaryOperators__f32[PropertyBinaryOperatorType::Store]			= dbg_new PropertyValueBinaryOperator__f32_Store{};
	PropertyStatics::BinaryOperators__f32[PropertyBinaryOperatorType::Move]			= dbg_new PropertyValueBinaryOperator__f32_Move{};
	PropertyStatics::BinaryOperators__f32[PropertyBinaryOperatorType::Plus]			= dbg_new PropertyValueBinaryOperator__f32_Plus{};
	PropertyStatics::BinaryOperators__f32[PropertyBinaryOperatorType::Minus]			= dbg_new PropertyValueBinaryOperator__f32_Minus{};
	PropertyStatics::BinaryOperators__f32[PropertyBinaryOperatorType::Multiply]		= dbg_new PropertyValueBinaryOperator__f32_Multiply{};
	PropertyStatics::BinaryOperators__f32[PropertyBinaryOperatorType::Divide]			= dbg_new PropertyValueBinaryOperator__f32_Divide{};
	PropertyStatics::BinaryOperators__f32[PropertyBinaryOperatorType::Modulus]			= dbg_new PropertyValueBinaryOperator__f32_Modulus{};
	PropertyStatics::BinaryOperators__f32[PropertyBinaryOperatorType::Equal]			= dbg_new PropertyValueBinaryOperator__f32_Equal{};
	PropertyStatics::BinaryOperators__f32[PropertyBinaryOperatorType::NotEqual]		= dbg_new PropertyValueBinaryOperator__f32_NotEqual{};
	PropertyStatics::BinaryOperators__f32[PropertyBinaryOperatorType::GreatorEqual]	= dbg_new PropertyValueBinaryOperator__f32_GreatorEqual{};
	PropertyStatics::BinaryOperators__f32[PropertyBinaryOperatorType::Greator]			= dbg_new PropertyValueBinaryOperator__f32_Greator{};
	PropertyStatics::BinaryOperators__f32[PropertyBinaryOperatorType::LessEqual]		= dbg_new PropertyValueBinaryOperator__f32_LessEqual{};
	PropertyStatics::BinaryOperators__f32[PropertyBinaryOperatorType::Less]			= dbg_new PropertyValueBinaryOperator__f32_Less{};
}

NS_END
