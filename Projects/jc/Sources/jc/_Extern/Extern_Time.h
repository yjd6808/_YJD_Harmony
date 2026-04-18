/*
 * 작성자 : 윤정도
 * 목적 : MASM 어셈블리 코드에서 Time(DateTime) API를 호출하기 위한 extern "C" 래퍼
 * created by AI.
 */

#pragma once

#ifdef __cplusplus
#include "jc/Type.h"
extern "C" {
#endif

/* ========================================================================
   기본
   ======================================================================== */

/*
 * 현재 시각을 DateTime Tick(_s64)으로 반환
 * @return 현재 로컬 시각의 Tick (마이크로초 단위)
 */
_s64 Time_Now();

/*
 * 주어진 Tick으로 DateTime을 생성
 * @param _year        : 년도
 * @param _month       : 월 (1~12)
 * @param _day         : 일 (1~31)
 * @param _hour        : 시 (0~23)
 * @param _minute      : 분 (0~59)
 * @param _second      : 초 (0~59)
 * @param _miliSecond  : 밀리초 (0~999)
 * @param _microSecond : 마이크로초 (0~999)
 * @return 생성된 DateTime의 Tick
 */
_s64 Time_Make(_s32 _year, _s32 _month, _s32 _day,
               _s32 _hour, _s32 _minute, _s32 _second,
               _s32 _miliSecond, _s32 _microSecond);

/*
 * DateTime을 지정된 포맷 문자열로 버퍼에 기록
 * @param _tick     : DateTime Tick
 * @param _fmt      : 포맷 문자열 (예: "yyyy-MM-dd HH:mm:ss")
 * @param _pBuff    : 출력 버퍼
 * @param _capacity : 버퍼 용량
 * @return 기록된 바이트 수
 */
int Time_FormatBuffered(_s64 _tick, const char* _fmt, char* _pBuff, int _capacity);

/*
 * 두 DateTime의 차이를 TimeSpan Tick(_s64)으로 반환
 * @param _lhsTick : 기준 DateTime Tick
 * @param _rhsTick : 빼는 DateTime Tick
 * @return _lhsTick - _rhsTick 의 TimeSpan Tick (마이크로초)
 */
_s64 Time_Diff(_s64 _lhsTick, _s64 _rhsTick);

/* ========================================================================
   Getter
   ======================================================================== */

/*
 * @param _tick : DateTime Tick
 * @return 년도
 */
int Time_GetYear(_s64 _tick);

/*
 * @param _tick : DateTime Tick
 * @return 월 (1~12)
 */
int Time_GetMonth(_s64 _tick);

/*
 * @param _tick : DateTime Tick
 * @return 일 (1~31)
 */
int Time_GetDay(_s64 _tick);

/*
 * @param _tick : DateTime Tick
 * @return 시 (0~23)
 */
int Time_GetHour(_s64 _tick);

/*
 * @param _tick : DateTime Tick
 * @return 분 (0~59)
 */
int Time_GetMinute(_s64 _tick);

/*
 * @param _tick : DateTime Tick
 * @return 초 (0~59)
 */
int Time_GetSecond(_s64 _tick);

/*
 * @param _tick : DateTime Tick
 * @return 밀리초 (0~999)
 */
int Time_GetMiliSecond(_s64 _tick);

/*
 * @param _tick : DateTime Tick
 * @return 마이크로초 (0~999)
 */
int Time_GetMicroSecond(_s64 _tick);

/* ========================================================================
   Add (새 DateTime Tick 반환)
   ======================================================================== */

_s64 Time_AddYear       (_s64 _tick, _s32 _value);
_s64 Time_AddMonth      (_s64 _tick, _s32 _value);
_s64 Time_AddDay        (_s64 _tick, _s32 _value);
_s64 Time_AddHour       (_s64 _tick, _s64 _value);
_s64 Time_AddMinute     (_s64 _tick, _s64 _value);
_s64 Time_AddSecond     (_s64 _tick, _s64 _value);
_s64 Time_AddMiliSecond (_s64 _tick, _s64 _value);
_s64 Time_AddMicroSecond(_s64 _tick, _s64 _value);

/* ========================================================================
   Subtract (새 DateTime Tick 반환)
   ======================================================================== */

_s64 Time_SubtractYear       (_s64 _tick, _s32 _value);
_s64 Time_SubtractMonth      (_s64 _tick, _s32 _value);
_s64 Time_SubtractDay        (_s64 _tick, _s32 _value);
_s64 Time_SubtractHour       (_s64 _tick, _s64 _value);
_s64 Time_SubtractMinute     (_s64 _tick, _s64 _value);
_s64 Time_SubtractSecond     (_s64 _tick, _s64 _value);
_s64 Time_SubtractMiliSecond (_s64 _tick, _s64 _value);
_s64 Time_SubtractMicroSecond(_s64 _tick, _s64 _value);

#ifdef __cplusplus
}
#endif
