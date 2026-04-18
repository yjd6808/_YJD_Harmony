/*
 * 작성자 : 윤정도
 * 목적 : MASM 어셈블리 코드에서 Time(DateTime) API를 호출하기 위한 extern "C" 래퍼 구현
 * created by AI.
 */

#include "Extern_Time.h"
#include "jc/Time.h"

USING_NS_JC;

/* ========================================================================
   기본
   ======================================================================== */

//////////////////////////////////////////////////////////////////////////////////////////
_s64 Time_Now()
{
    return (_s64)DateTime::Now().Tick;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s64 Time_Make(_s32 _year, _s32 _month, _s32 _day,
               _s32 _hour, _s32 _minute, _s32 _second,
               _s32 _miliSecond, _s32 _microSecond)
{
    return (_s64)DateAndTime(_year, _month, _day, _hour, _minute, _second, _miliSecond, _microSecond).ToDateTime().Tick;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Time_FormatBuffered(_s64 _tick, const char* _fmt, char* _pBuff, int _capacity)
{
    return DateTime(_tick).FormatBuffered(_fmt, _pBuff, _capacity);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s64 Time_Diff(_s64 _lhsTick, _s64 _rhsTick)
{
    return DateTime(_lhsTick).Diff(DateTime(_rhsTick)).Tick;
}

/* ========================================================================
   Getter
   ======================================================================== */

//////////////////////////////////////////////////////////////////////////////////////////
int Time_GetYear(_s64 _tick)        { return DateTime(_tick).GetYear(); }
int Time_GetMonth(_s64 _tick)       { return DateTime(_tick).GetMonth(); }
int Time_GetDay(_s64 _tick)         { return DateTime(_tick).GetDay(); }
int Time_GetHour(_s64 _tick)        { return DateTime(_tick).GetHour(); }
int Time_GetMinute(_s64 _tick)      { return DateTime(_tick).GetMinute(); }
int Time_GetSecond(_s64 _tick)      { return DateTime(_tick).GetSecond(); }
int Time_GetMiliSecond(_s64 _tick)  { return DateTime(_tick).GetMiliSecond(); }
int Time_GetMicroSecond(_s64 _tick) { return DateTime(_tick).GetMicroSecond(); }

/* ========================================================================
   Add
   ======================================================================== */

//////////////////////////////////////////////////////////////////////////////////////////
_s64 Time_AddYear       (_s64 _tick, _s32 _value) { return (_s64)DateTime(_tick).AddYear(_value).Tick; }
_s64 Time_AddMonth      (_s64 _tick, _s32 _value) { return (_s64)DateTime(_tick).AddMonth(_value).Tick; }
_s64 Time_AddDay        (_s64 _tick, _s32 _value) { return (_s64)DateTime(_tick).AddDay(_value).Tick; }
_s64 Time_AddHour       (_s64 _tick, _s64 _value) { return (_s64)DateTime(_tick).AddHour(_value).Tick; }
_s64 Time_AddMinute     (_s64 _tick, _s64 _value) { return (_s64)DateTime(_tick).AddMinute(_value).Tick; }
_s64 Time_AddSecond     (_s64 _tick, _s64 _value) { return (_s64)DateTime(_tick).AddSecond(_value).Tick; }
_s64 Time_AddMiliSecond (_s64 _tick, _s64 _value) { return (_s64)DateTime(_tick).AddMiliSecond(_value).Tick; }
_s64 Time_AddMicroSecond(_s64 _tick, _s64 _value) { return (_s64)DateTime(_tick).AddMicroSecond(_value).Tick; }

/* ========================================================================
   Subtract
   ======================================================================== */

//////////////////////////////////////////////////////////////////////////////////////////
_s64 Time_SubtractYear       (_s64 _tick, _s32 _value) { return (_s64)DateTime(_tick).SubtractYear(_value).Tick; }
_s64 Time_SubtractMonth      (_s64 _tick, _s32 _value) { return (_s64)DateTime(_tick).SubtractMonth(_value).Tick; }
_s64 Time_SubtractDay        (_s64 _tick, _s32 _value) { return (_s64)DateTime(_tick).SubtractDay(_value).Tick; }
_s64 Time_SubtractHour       (_s64 _tick, _s64 _value) { return (_s64)DateTime(_tick).SubtractHour(_value).Tick; }
_s64 Time_SubtractMinute     (_s64 _tick, _s64 _value) { return (_s64)DateTime(_tick).SubtractMinute(_value).Tick; }
_s64 Time_SubtractSecond     (_s64 _tick, _s64 _value) { return (_s64)DateTime(_tick).SubtractSecond(_value).Tick; }
_s64 Time_SubtractMiliSecond (_s64 _tick, _s64 _value) { return (_s64)DateTime(_tick).SubtractMiliSecond(_value).Tick; }
_s64 Time_SubtractMicroSecond(_s64 _tick, _s64 _value) { return (_s64)DateTime(_tick).SubtractMicroSecond(_value).Tick; }
