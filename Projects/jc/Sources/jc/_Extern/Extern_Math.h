/*
 * 작성자 : 윤정도
 * 목적 : MASM 어셈블리 코드에서 Math API를 호출하기 위한 extern "C" 래퍼
 * created by AI.
 */

#pragma once

#ifdef __cplusplus
#include "jc/Type.h"
extern "C" {
#endif

// ========================================================================
// Pow
// ========================================================================

/*
 * 거듭제곱 (S32)
 * @param _base     : 밑 값
 * @param _exponent : 지수 값
 * @return _base ^ _exponent
 */
_s32 Math_PowS32(_s32 _base, _s32 _exponent);

/*
 * 거듭제곱 (U32)
 * @param _base     : 밑 값
 * @param _exponent : 지수 값
 * @return _base ^ _exponent
 */
_u32 Math_PowU32(_u32 _base, _s32 _exponent);

/*
 * 거듭제곱 (S64)
 * @param _base     : 밑 값
 * @param _exponent : 지수 값
 * @return _base ^ _exponent
 */
_s64 Math_PowS64(_s64 _base, _s32 _exponent);

/*
 * 거듭제곱 (U64)
 * @param _base     : 밑 값
 * @param _exponent : 지수 값
 * @return _base ^ _exponent
 */
_u64 Math_PowU64(_u64 _base, _s32 _exponent);

/*
 * 거듭제곱 (float)
 * @param _base     : 밑 값
 * @param _exponent : 지수 값
 * @return _base ^ _exponent
 */
_f32 Math_PowFloat(_f32 _base, _s32 _exponent);

/*
 * 거듭제곱 (double)
 * @param _base     : 밑 값
 * @param _exponent : 지수 값
 * @return _base ^ _exponent
 */
_f64 Math_PowDouble(_f64 _base, _s32 _exponent);

// ========================================================================
// Max
// ========================================================================

/*
 * 두 값 중 최댓값 반환 (S32)
 * @param _lhs : 첫 번째 값
 * @param _rhs : 두 번째 값
 * @return 최댓값
 */
_s32 Math_MaxS32(_s32 _lhs, _s32 _rhs);

/*
 * 두 값 중 최댓값 반환 (U32)
 * @param _lhs : 첫 번째 값
 * @param _rhs : 두 번째 값
 * @return 최댓값
 */
_u32 Math_MaxU32(_u32 _lhs, _u32 _rhs);

/*
 * 두 값 중 최댓값 반환 (S64)
 * @param _lhs : 첫 번째 값
 * @param _rhs : 두 번째 값
 * @return 최댓값
 */
_s64 Math_MaxS64(_s64 _lhs, _s64 _rhs);

/*
 * 두 값 중 최댓값 반환 (U64)
 * @param _lhs : 첫 번째 값
 * @param _rhs : 두 번째 값
 * @return 최댓값
 */
_u64 Math_MaxU64(_u64 _lhs, _u64 _rhs);

/*
 * 두 값 중 최댓값 반환 (float)
 * @param _lhs : 첫 번째 값
 * @param _rhs : 두 번째 값
 * @return 최댓값
 */
_f32 Math_MaxFloat(_f32 _lhs, _f32 _rhs);

/*
 * 두 값 중 최댓값 반환 (double)
 * @param _lhs : 첫 번째 값
 * @param _rhs : 두 번째 값
 * @return 최댓값
 */
_f64 Math_MaxDouble(_f64 _lhs, _f64 _rhs);

// ========================================================================
// Max3
// ========================================================================

/*
 * 세 값 중 최댓값 반환 (S32)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @return 최댓값
 */
_s32 Math_Max3S32(_s32 _a, _s32 _b, _s32 _c);

/*
 * 세 값 중 최댓값 반환 (U32)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @return 최댓값
 */
_u32 Math_Max3U32(_u32 _a, _u32 _b, _u32 _c);

/*
 * 세 값 중 최댓값 반환 (S64)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @return 최댓값
 */
_s64 Math_Max3S64(_s64 _a, _s64 _b, _s64 _c);

/*
 * 세 값 중 최댓값 반환 (U64)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @return 최댓값
 */
_u64 Math_Max3U64(_u64 _a, _u64 _b, _u64 _c);

/*
 * 세 값 중 최댓값 반환 (float)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @return 최댓값
 */
_f32 Math_Max3Float(_f32 _a, _f32 _b, _f32 _c);

/*
 * 세 값 중 최댓값 반환 (double)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @return 최댓값
 */
_f64 Math_Max3Double(_f64 _a, _f64 _b, _f64 _c);

// ========================================================================
// Max4
// ========================================================================

/*
 * 네 값 중 최댓값 반환 (S32)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @param _d : 네 번째 값
 * @return 최댓값
 */
_s32 Math_Max4S32(_s32 _a, _s32 _b, _s32 _c, _s32 _d);

/*
 * 네 값 중 최댓값 반환 (U32)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @param _d : 네 번째 값
 * @return 최댓값
 */
_u32 Math_Max4U32(_u32 _a, _u32 _b, _u32 _c, _u32 _d);

/*
 * 네 값 중 최댓값 반환 (S64)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @param _d : 네 번째 값
 * @return 최댓값
 */
_s64 Math_Max4S64(_s64 _a, _s64 _b, _s64 _c, _s64 _d);

/*
 * 네 값 중 최댓값 반환 (U64)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @param _d : 네 번째 값
 * @return 최댓값
 */
_u64 Math_Max4U64(_u64 _a, _u64 _b, _u64 _c, _u64 _d);

/*
 * 네 값 중 최댓값 반환 (float)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @param _d : 네 번째 값
 * @return 최댓값
 */
_f32 Math_Max4Float(_f32 _a, _f32 _b, _f32 _c, _f32 _d);

/*
 * 네 값 중 최댓값 반환 (double)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @param _d : 네 번째 값
 * @return 최댓값
 */
_f64 Math_Max4Double(_f64 _a, _f64 _b, _f64 _c, _f64 _d);

// ========================================================================
// Min
// ========================================================================

/*
 * 두 값 중 최솟값 반환 (S32)
 * @param _lhs : 첫 번째 값
 * @param _rhs : 두 번째 값
 * @return 최솟값
 */
_s32 Math_MinS32(_s32 _lhs, _s32 _rhs);

/*
 * 두 값 중 최솟값 반환 (U32)
 * @param _lhs : 첫 번째 값
 * @param _rhs : 두 번째 값
 * @return 최솟값
 */
_u32 Math_MinU32(_u32 _lhs, _u32 _rhs);

/*
 * 두 값 중 최솟값 반환 (S64)
 * @param _lhs : 첫 번째 값
 * @param _rhs : 두 번째 값
 * @return 최솟값
 */
_s64 Math_MinS64(_s64 _lhs, _s64 _rhs);

/*
 * 두 값 중 최솟값 반환 (U64)
 * @param _lhs : 첫 번째 값
 * @param _rhs : 두 번째 값
 * @return 최솟값
 */
_u64 Math_MinU64(_u64 _lhs, _u64 _rhs);

/*
 * 두 값 중 최솟값 반환 (float)
 * @param _lhs : 첫 번째 값
 * @param _rhs : 두 번째 값
 * @return 최솟값
 */
_f32 Math_MinFloat(_f32 _lhs, _f32 _rhs);

/*
 * 두 값 중 최솟값 반환 (double)
 * @param _lhs : 첫 번째 값
 * @param _rhs : 두 번째 값
 * @return 최솟값
 */
_f64 Math_MinDouble(_f64 _lhs, _f64 _rhs);

// ========================================================================
// Min3
// ========================================================================

/*
 * 세 값 중 최솟값 반환 (S32)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @return 최솟값
 */
_s32 Math_Min3S32(_s32 _a, _s32 _b, _s32 _c);

/*
 * 세 값 중 최솟값 반환 (U32)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @return 최솟값
 */
_u32 Math_Min3U32(_u32 _a, _u32 _b, _u32 _c);

/*
 * 세 값 중 최솟값 반환 (S64)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @return 최솟값
 */
_s64 Math_Min3S64(_s64 _a, _s64 _b, _s64 _c);

/*
 * 세 값 중 최솟값 반환 (U64)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @return 최솟값
 */
_u64 Math_Min3U64(_u64 _a, _u64 _b, _u64 _c);

/*
 * 세 값 중 최솟값 반환 (float)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @return 최솟값
 */
_f32 Math_Min3Float(_f32 _a, _f32 _b, _f32 _c);

/*
 * 세 값 중 최솟값 반환 (double)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @return 최솟값
 */
_f64 Math_Min3Double(_f64 _a, _f64 _b, _f64 _c);

// ========================================================================
// Min4
// ========================================================================

/*
 * 네 값 중 최솟값 반환 (S32)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @param _d : 네 번째 값
 * @return 최솟값
 */
_s32 Math_Min4S32(_s32 _a, _s32 _b, _s32 _c, _s32 _d);

/*
 * 네 값 중 최솟값 반환 (U32)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @param _d : 네 번째 값
 * @return 최솟값
 */
_u32 Math_Min4U32(_u32 _a, _u32 _b, _u32 _c, _u32 _d);

/*
 * 네 값 중 최솟값 반환 (S64)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @param _d : 네 번째 값
 * @return 최솟값
 */
_s64 Math_Min4S64(_s64 _a, _s64 _b, _s64 _c, _s64 _d);

/*
 * 네 값 중 최솟값 반환 (U64)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @param _d : 네 번째 값
 * @return 최솟값
 */
_u64 Math_Min4U64(_u64 _a, _u64 _b, _u64 _c, _u64 _d);

/*
 * 네 값 중 최솟값 반환 (float)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @param _d : 네 번째 값
 * @return 최솟값
 */
_f32 Math_Min4Float(_f32 _a, _f32 _b, _f32 _c, _f32 _d);

/*
 * 네 값 중 최솟값 반환 (double)
 * @param _a : 첫 번째 값
 * @param _b : 두 번째 값
 * @param _c : 세 번째 값
 * @param _d : 네 번째 값
 * @return 최솟값
 */
_f64 Math_Min4Double(_f64 _a, _f64 _b, _f64 _c, _f64 _d);

// ========================================================================
// Clamp
// ========================================================================

/*
 * 값을 범위로 제한 (S32)
 * @param _value : 입력 값
 * @param _min   : 최솟값 (포함)
 * @param _max   : 최댓값 (포함)
 * @return 범위 내로 제한된 값
 */
_s32 Math_ClampS32(_s32 _value, _s32 _min, _s32 _max);

/*
 * 값을 범위로 제한 (U32)
 * @param _value : 입력 값
 * @param _min   : 최솟값 (포함)
 * @param _max   : 최댓값 (포함)
 * @return 범위 내로 제한된 값
 */
_u32 Math_ClampU32(_u32 _value, _u32 _min, _u32 _max);

/*
 * 값을 범위로 제한 (S64)
 * @param _value : 입력 값
 * @param _min   : 최솟값 (포함)
 * @param _max   : 최댓값 (포함)
 * @return 범위 내로 제한된 값
 */
_s64 Math_ClampS64(_s64 _value, _s64 _min, _s64 _max);

/*
 * 값을 범위로 제한 (U64)
 * @param _value : 입력 값
 * @param _min   : 최솟값 (포함)
 * @param _max   : 최댓값 (포함)
 * @return 범위 내로 제한된 값
 */
_u64 Math_ClampU64(_u64 _value, _u64 _min, _u64 _max);

/*
 * 값을 범위로 제한 (float)
 * @param _value : 입력 값
 * @param _min   : 최솟값 (포함)
 * @param _max   : 최댓값 (포함)
 * @return 범위 내로 제한된 값
 */
_f32 Math_ClampFloat(_f32 _value, _f32 _min, _f32 _max);

/*
 * 값을 범위로 제한 (double)
 * @param _value : 입력 값
 * @param _min   : 최솟값 (포함)
 * @param _max   : 최댓값 (포함)
 * @return 범위 내로 제한된 값
 */
_f64 Math_ClampDouble(_f64 _value, _f64 _min, _f64 _max);

#ifdef __cplusplus
}
#endif
