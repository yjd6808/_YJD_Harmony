/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 10:25:34 PM
 * =====================
 *
 */

#include "PropertyStatics.h"
#include "PropertyOperatorDefinitionMacro.h"

NS_JC_BEGIN

SG_OPERATION_DEF_BEGIN(_f64l, Store)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs = (LTy)rhs)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs = (LTy)rhs)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f64l, Move)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs = (LTy)rhs)
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs = (LTy)rhs)
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs, LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f64l, Plus)
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
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f64l, Minus)
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
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f64l, Multiply)
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
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

// _s32 / long long이 데이터 손실날 경우는 뭐가있을까?
SG_OPERATION_DEF_BEGIN(_f64l, Divide)
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
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f64l, Modulus)
SG_OPERATION_OPERATE(const _s64&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f64l(rhs)), )
SG_OPERATION_OPERATE(const _u64&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f64l(rhs)), )
SG_OPERATION_OPERATE(const _s32&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f64l(rhs)), )
SG_OPERATION_OPERATE(const _u32&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f64l(rhs)), )
SG_OPERATION_OPERATE(const _s32l&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f64l(rhs)), )
SG_OPERATION_OPERATE(const _u32l&)	SG_OPERATION_BODY(lhs = fmod(lhs, _f64l(rhs)), )
SG_OPERATION_OPERATE(const _s16&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f64l(rhs)), )
SG_OPERATION_OPERATE(const _u16&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f64l(rhs)), )
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f64l(rhs)), LogOperation)
SG_OPERATION_OPERATE(const _s8&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f64l(rhs)), )
SG_OPERATION_OPERATE(const _u8&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f64l(rhs)), )
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f64l(rhs)), )
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(lhs = fmod(lhs, _f64l(rhs)), )
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(lhs = fmod(lhs, _f64l(rhs)), )
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f64l, Equal)
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

SG_OPERATION_DEF_BEGIN(_f64l, NotEqual)
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
SG_OPERATION_OPERATE(const _f32&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _f64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const _f64l&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != (LTy)rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(_f64l, GreatorEqual)
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

SG_OPERATION_DEF_BEGIN(_f64l, Greator)
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

SG_OPERATION_DEF_BEGIN(_f64l, LessEqual)
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

SG_OPERATION_DEF_BEGIN(_f64l, Less)
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

void InitializePropertyOperatorLDouble()
{
	PropertyStatics::BinaryOperators__f64l[PropertyBinaryOperatorType::Store]			= dbg_new PropertyValueBinaryOperator__f64l_Store{};
	PropertyStatics::BinaryOperators__f64l[PropertyBinaryOperatorType::Move]			= dbg_new PropertyValueBinaryOperator__f64l_Move{};
	PropertyStatics::BinaryOperators__f64l[PropertyBinaryOperatorType::Plus]			= dbg_new PropertyValueBinaryOperator__f64l_Plus{};
	PropertyStatics::BinaryOperators__f64l[PropertyBinaryOperatorType::Minus]			= dbg_new PropertyValueBinaryOperator__f64l_Minus{};
	PropertyStatics::BinaryOperators__f64l[PropertyBinaryOperatorType::Multiply]		= dbg_new PropertyValueBinaryOperator__f64l_Multiply{};
	PropertyStatics::BinaryOperators__f64l[PropertyBinaryOperatorType::Divide]		= dbg_new PropertyValueBinaryOperator__f64l_Divide{};
	PropertyStatics::BinaryOperators__f64l[PropertyBinaryOperatorType::Modulus]		= dbg_new PropertyValueBinaryOperator__f64l_Modulus{};
	PropertyStatics::BinaryOperators__f64l[PropertyBinaryOperatorType::Equal]			= dbg_new PropertyValueBinaryOperator__f64l_Equal{};
	PropertyStatics::BinaryOperators__f64l[PropertyBinaryOperatorType::NotEqual]		= dbg_new PropertyValueBinaryOperator__f64l_NotEqual{};
	PropertyStatics::BinaryOperators__f64l[PropertyBinaryOperatorType::GreatorEqual]	= dbg_new PropertyValueBinaryOperator__f64l_GreatorEqual{};
	PropertyStatics::BinaryOperators__f64l[PropertyBinaryOperatorType::Greator]		= dbg_new PropertyValueBinaryOperator__f64l_Greator{};
	PropertyStatics::BinaryOperators__f64l[PropertyBinaryOperatorType::LessEqual]		= dbg_new PropertyValueBinaryOperator__f64l_LessEqual{};
	PropertyStatics::BinaryOperators__f64l[PropertyBinaryOperatorType::Less]			= dbg_new PropertyValueBinaryOperator__f64l_Less{};
}

NS_END