/*
	작성자 : 윤정도
	간단한 시간을 다룰 수 있는 기능을 추가합니다.
	스톱워치 기능

	시간 관련지식이 전무해서
	C# DateTime 클래스의 년도 계산 로직 일부를 참고하였습니다.
	구현다보니 좀 자세히 알게됨 ㅋ
*/

#pragma once

#include "jc/Type.h"
#include "jc/Tuple.h"
#include "jc/Comparator.h"
#include "jc/Primitives/BitFlag.h"
#include "jc/Container/Vector.h"


NS_JC_BEGIN
	NS_DETAIL_BEGIN

	constexpr _s64 MaxDay_v			= 365;
	constexpr _s64 MaxHour_v			= 24;
	constexpr _s64 MaxMinute_v			= 60;
	constexpr _s64 MaxSecond_v			= 60;
	constexpr _s64 MaxMiliSecond_v		= 1000;
	constexpr _s64 MaxMicroSecond_v	= 1000;

	constexpr float MaxDayf_v			= 365.f;
	constexpr float MaxHourf_v			= 24.f;
	constexpr float MaxMinutef_v		= 60.f;
	constexpr float MaxSecondf_v		= 60.f;
	constexpr float MaxMiliSecondf_v	= 1000.f;
	constexpr float MaxMicroSecondf_v	= 1000.f;
		
	// 1틱당 1마이크로초
	constexpr _s64 TicksPerMicroSecond_v	= 1;										// 마이크로초당 몇 틱인지 	1틱당					1마이크로초
	constexpr _s64 TicksPerMiliSecond_v	= TicksPerMicroSecond_v * MaxMicroSecond_v;	// 밀리초당 몇 틱인지		1000틱당					1밀리초
	constexpr _s64 TicksPerSecond_v		= TicksPerMiliSecond_v * MaxMiliSecond_v;		// 초당 몇 틱인지			1000000틱당				1초
	constexpr _s64 TicksPerMinute_v		= TicksPerSecond_v * MaxSecond_v;				// 1분당 몇 틱인지		1000000 * 60			1분
	constexpr _s64 TicksPerHour_v		    = TicksPerMinute_v * MaxMinute_v;				// 1시간당 몇 틱인지		1000000 * 60 * 60		1시간
	constexpr _s64 TicksPerDay_v			= TicksPerHour_v * MaxHour_v;					// 1일당 몇 틱인지		1000000 * 60 * 60 * 24	1일

	constexpr float SecondsPerMicroSecondf_v = 0.000001f;
	constexpr float SecondsPerMiliSecondf_v = 0.001f;
	constexpr float SecondsPerSecondf_v = 1.0f;
	constexpr float SecondsPerMinutef_v = SecondsPerSecondf_v * MaxSecondf_v;
	constexpr float SecondsPerHourf_v = SecondsPerMinutef_v * MaxMinutef_v;
	constexpr float SecondsPerDayf_v = SecondsPerHourf_v * MaxHourf_v;

	constexpr int DaysPer1Years_v = MaxDay_v;
	constexpr int DaysPer4Years_v = MaxDay_v * 4 + 1;				// 4년이 몇일인지 : 1461일
	constexpr int DaysPer100Years_v = DaysPer4Years_v * 25 - 1;		// 100년이 몇일인지 : 36524일
	constexpr int DaysPer400Years_v = DaysPer100Years_v * 4 + 1;	// 400년이 몇일인지 : 146097일
		
	constexpr int DaysUntilMonth365_v[13] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
	constexpr int DaysUntilMonth366_v[13] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

	constexpr int DaysForMonth365_v[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	constexpr int DaysForMonth366_v[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	constexpr _s64 Ticks10000Years_v = DaysPer400Years_v * 25 * TicksPerDay_v;



	/*
		After Christ 1970년도까지의 시간을 구한다음에 해봤는데 오차가 생김;
		아래 처럼 일 수의 소수점 단위까지 들고가면 안된다.
		constexpr long double EpochTimeBeginAC = long double(DaysPer400Years_v * 1969) / 400; // 일 수의 소수점 단위는 버려야함
	*/
		
	// [AD 0001년 1월 1일 ~ 1969년 12월 31일까지의 마이크로초 계산]
	constexpr _s64 ADBegin_v	  = (DaysPer400Years_v * 1969) / 400;	// Epoch 시간 1970년 1월 1일까지의 일 수
	constexpr _s64 ADBeginTick_v = ADBegin_v * TicksPerDay_v;			// Epoch 시간 1970년 1월 1일까지의 마이크로초

	NS_END


enum class DatePart
{
	Year,			// 몇년도인지				1 ~ 9999
	Month,			// 1년중에 몇월인지		1 ~ 12
	Day,			// 현재달에 몇일인지		1 ~ (28 or 29 or 30 or 31)
	DayOfYear,		// 1년중에 몇번째 일인지	1 ~ (355 or 366)
	MaxDayOfMonth	// 이번에 최대 몇일		(28 or 29 or 30 or 31)
};

enum class DayOfWeek
{
	Monday,
	Tuesday,
	Wednesday,
	Thursdays,
	Friday,
	Saturday,
};

enum class MonthOfYear
{
	January,
	February,
	March,
	April,
	June,
	July,
	August,
	September,
	October,
	November,
	December
};

enum class AMPM
{
	AM,
	PM,
	None
};

enum class TimeStandard
{
	Local,			// 현재 컴퓨터 시스템 기준 시간
	Universal		// UTC 기준시간
};

// https://www.c-sharpcorner.com/blogs/date-and-time-format-in-c-sharp-programming1

JC_SENUM_BEGIN(DateFormat)
d,			// 일 표시 -> 1 ~ 31
dd,			// 일 표시 -> 01 ~ 31
ddd,		// 요일 축약 이름-> Mon, Tue, Wed 
dddd,		// 요일 전체 이름 -> Monday, Tuesday, Wednesday
h,			// 12시간 단위 시간 -> 2
hh,			// 12시간 단위 시간 -> 02
H,			// 24시간 단위 시간 -> 2, 23
HH,			// 24시간 단위 시간 -> 02, 23
m,			// 분 -> 5
mm,			// 분 -> 05
M,			// 달 -> 1
MM,			// 달 -> 01
MMM,		// 달 축약 이름 -> Dec, Sep
MMMM,		// 달 전체 이름 -> December
s,			// 초 -> 5초
ss,			// 초 -> 05초
t,			// 오전 오후 -> A, P
tt,			// 오전 오후 -> AM, PM
y,			// 2자리년도 -> 2009 -> 9
yy,			// 2자리년도 -> 2009 -> 09
yyy,		// 4자리년도 -> 2022 -> 2022
yyyy,		// 4자리년도 -> 2022 -> 2022
K,			// 타임존 시간 (시간:분) -> +09:00
z,			// 타임존 시간 (시간) -> +9
zz,			// 타임존 시간 (시간) -> +09
zzz,		// 타임존 시간 (시간:분) -> +09:00
f,			// 밀리초 100의 자리까지 표시		10.123456 -> 1
ff,			// 밀리초 10의 자리까지 표시		10.123456 -> 12
fff,		// 밀리초 1의 자리까지 표시			10.123456 -> 123
ffff,		// 마이크로초 100의 자리까지 표시	10.123456 -> 1234
fffff,		// 마이크로초 10의 자리까지 표시	10.123456 -> 12345
ffffff,		// 마이크로초 1의 자리까지 표시		10.123456 -> 123456
JC_SENUM_MIDDLE(DateFormat)
static constexpr const char* ToString[] = {
		"d",
		"dd",
		"ddd",
		"dddd",
		"h",
		"hh",
		"H",
		"HH",
		"m",
		"mm",
		"M",
		"MM",
		"MMM",
		"MMMM",
		"s",
		"ss",
		"t",
		"tt",
		"y",
		"yy",
		"yyy",
		"yyyy",
		"K",
		"z",
		"zz",
		"zzz",
		"f",
		"ff",
		"fff",
		"ffff",
		"fffff",
		"ffffff"
};
JC_SENUM_MIDDLE_END(DateFormat)

/* =================================================================================== 
	시간 단위
	=================================================================================== */

enum class TimeUnit
{
	Year,
	Month,
	Day,
	Hour,
	Minute,
	Second,
	MiliSecond,
	MicroSecond,
	NanoSecond
};

		
/* =================================================================================== 
	시간 계산 및 관리

	Date와 Time, DateAndTime의 DateTimedml 덧셈 뺄셈 연산 수행결과로 날짜, 시간, 틱이 음수가 나올 경우 예외를 던지므로
	음수의 시간 차이를 계산하고 싶은 경우에는 DateTime의 Diff 함수를 사용하여 TimeSpan 구조체를 통해 확인할 것
	
	=================================================================================== */
struct Date
{
public:
	Date()
		: Year(0)
		, Month(0)
		, Day(0)
	{}
	Date(_s32 _year, _s32 _month, _s32 _day);
	Date(const Date& _other) : Year(_other.Year), Month(_other.Month), Day(_other.Day) {}

	virtual void AddYear(_s32 _years);
	virtual void AddMonth(_s32 _months);
	virtual void AddDay(_s32 _days);
	virtual void AddDate(const Date& _other);

	virtual void SubtractYear(_s32 _years);
	virtual void SubtractMonth(_s32 _years);
	virtual void SubtractDay(_s32 _years);
	virtual void SubtractDate(const Date& _other);

	_s32 GetYear() const { return _s32(Year); }
	_s32 GetMonth() const { return _s32(Month); }
	_s32 GetDay() const { return _s32(Day); }

	int Compare(const Date& _other) const { return Comparator<_s64>()(ToTick(), _other.ToTick()); }

	Date operator-(const Date& _other) const;
	Date operator+(const Date& _other) const;
	Date& operator-=(const Date& _other);
	Date& operator+=(const Date& _other);
	bool operator>(const Date& _other);
	bool operator<(const Date& _other);
	bool operator>=(const Date& _other);
	bool operator<=(const Date& _other);
	bool operator==(const Date& _other);

	_s64 ToTick() const;

	_s16	Year;
	_s8	Month;
	_s8	Day;

	friend class DateTime;
};





/*=====================================================================================
									Time
 =====================================================================================*/
struct Time
{
public:
	Time()
	: Hour(0)
	, Minute(0)
	, Second(0)
	, MiliSecond(0)
	, MicroSecond(0)
	{}
	Time(_s32 _hour, _s32 _minute, _s32 _second, _s32 _miliSecond, _s32 _microSecond);
	Time(const Time& _other) : Hour(_other.Hour), Minute(_other.Minute), Second(_other.Second), MiliSecond(_other.MiliSecond), MicroSecond(_other.MicroSecond) {}

	virtual void AddHour(_s64 _hours);
	virtual void AddMinute(_s64 _minutes);
	virtual void AddSecond(_s64 _seconds);
	virtual void AddMiliSecond(_s64 _miliSeconds);
	virtual void AddMicroSecond(_s64 _microSeconds, TimeUnit _timeUnit = TimeUnit::MicroSecond);
	virtual void AddTime(const Time& _other);

	virtual void SubtractHour(_s64 _hours);
	virtual void SubtractMinute(_s64 _minutes);
	virtual void SubtractSecond(_s64 _seconds);
	virtual void SubtractMiliSecond(_s64 _miliSeconds);
	virtual void SubtractMicroSecond(_s64 _microSeconds, TimeUnit _timeUnit = TimeUnit::MicroSecond);
	virtual void SubtractTime(const Time& _other);

	_s32 GetHour() const { return Hour; }
	_s32 GetMinute() const { return Minute; }
	_s32 GetSecond() const { return Second; }
	_s32 GetMiliSecond() const { return MiliSecond; }
	_s32 GetMicroSecond() const { return MicroSecond; }

	int Compare(const Time& _other) const { return Comparator<_s64>()(ToTick(), _other.ToTick()); }

	Time operator-(const Time& _other) const;
	Time operator+(const Time& _other) const;
	Time& operator-=(const Time& _other);
	Time& operator+=(const Time& _other);
	bool operator>(const Time& _other) const;
	bool operator<(const Time& _other) const;
	bool operator>=(const Time& _other) const;
	bool operator<=(const Time& _other) const;
	bool operator==(const Time& _other) const;


	_s64 ToTick() const;

	_s8 Hour;
	_s8 Minute;
	_s8 Second;
	_s16 MiliSecond;
	_s16 MicroSecond;

	friend class DateTime;
};


/*=====================================================================================
									DateAndTime
 =====================================================================================*/
class DateTime;
struct DateAndTime : Date, Time {
	DateAndTime() = default;
	DateAndTime(_s32 _year, _s32 _month, _s32 _day, _s32 _hour, _s32 _minute, _s32 _second, _s32 _miliSecond, _s32 _microSecond)
	: Date(_year, _month, _day)
	, Time(_hour, _minute, _second, _miliSecond, _microSecond)
	{}

	void AddYear(_s32 _years) override;				// Date::AddYear와 동일
	void AddMonth(_s32 _months) override;			// Date::AddMonth와 동일
	void AddDay(_s32 _days) override;				
	void AddHour(_s64 _hours) override;				
	void AddMinute(_s64 _minutes) override;			
	void AddSecond(_s64 _seconds) override;			
	void AddMiliSecond(_s64 _miliSeconds) override;	
	void AddMicroSecond(_s64 _microSeconds, TimeUnit _timeUnit = TimeUnit::MicroSecond) override;
	void AddDate(const Date& _other) override;
	void AddTime(const Time& _other) override;
	void AddDateAndTime(const DateAndTime& _other);

	void SubtractYear(_s32 _years) override;
	void SubtractMonth(_s32 _years) override;
	void SubtractDay(_s32 _years) override;
	void SubtractHour(_s64 _hours) override;
	void SubtractMinute(_s64 _minutes) override;
	void SubtractSecond(_s64 _seconds) override;
	void SubtractMiliSecond(_s64 _miliSeconds) override;
	void SubtractMicroSecond(_s64 _microSeconds, TimeUnit _timeUnit = TimeUnit::MicroSecond) override;
	void SubtractDate(const Date& _other) override;
	void SubtractTime(const Time& _other) override;
	void SubtractDateAndTime(const DateAndTime& _other);

	int Compare(const DateAndTime& _other) const { return Comparator<_s64>()(ToTick(), _other.ToTick()); }

	DateAndTime operator-(const DateAndTime& _other) const;
	DateAndTime operator+(const DateAndTime& _other) const;
	DateAndTime& operator-=(const DateAndTime& _other);
	DateAndTime& operator+=(const DateAndTime& _other);

	DateAndTime operator-(const DateTime& _other) const;
	DateAndTime operator+(const DateTime& _other) const;
	DateAndTime& operator-=(const DateTime& _other);
	DateAndTime& operator+=(const DateTime& _other);

	bool operator>(const DateAndTime& _other);
	bool operator<(const DateAndTime& _other);
	bool operator>=(const DateAndTime& _other);
	bool operator<=(const DateAndTime& _other);
	bool operator==(const DateAndTime& _other);

	bool operator>(const DateTime& _other) const;
	bool operator<(const DateTime& _other) const;
	bool operator>=(const DateTime& _other) const;
	bool operator<=(const DateTime& _other) const;
	bool operator==(const DateTime& _other) const;

	DateTime ToDateTime() const;
	_s64 ToTick() const;
};



/*=====================================================================================
								 TimeSpan
 =====================================================================================*/


// 음수 시간을 다룰 수 있는 구조체 (마이크로초단위 자릿수, 마이크로초단위 정밀도)
struct TimeSpan
{
	TimeSpan(_s64 _tick = 0) : Tick(_tick) {}
	TimeSpan(_s32 _days, _s64 _hours, _s64 _minutes, _s64 _seconds, _s64 _miliSeconds, _s64 _microSeconds);

	void SetZero() { Tick = 0; }

	double GetTotalDays() const { return (double)Tick / detail::TicksPerDay_v; }
	double GetTotalHours() const { return (double)Tick / detail::TicksPerHour_v; }
	double GetTotalMinutes() const { return (double)Tick / detail::TicksPerMinute_v; }
	double GetTotalSeconds() const { return (double)Tick / detail::TicksPerSecond_v; }
	double GetTotalMiliSeconds() const { return (double)Tick / detail::TicksPerMiliSecond_v; }
	double GetTotalMicroSeconds() const { return (double)Tick; }

	_s64 GetTotalDaysInt() const { return Tick / detail::TicksPerDay_v; }
	_s64 GetTotalHoursInt() const { return Tick / detail::TicksPerHour_v; }
	_s64 GetTotalMinutesInt() const { return Tick / detail::TicksPerMinute_v; }
	_s64 GetTotalSecondsInt() const { return Tick / detail::TicksPerSecond_v; }
	_s64 GetTotalMiliSecondsInt() const { return Tick / detail::TicksPerMiliSecond_v; }
	_s64 GetTotalMicroSecondsInt() const { return Tick; }

	_s32 GetTotalDaysInt32() const { return _s32(Tick / detail::TicksPerDay_v); }
	_s32 GetTotalHoursInt32() const { return _s32(Tick / detail::TicksPerHour_v); }
	_s32 GetTotalMinutesInt32() const { return _s32(Tick / detail::TicksPerMinute_v); }
	_s32 GetTotalSecondsInt32() const { return _s32(Tick / detail::TicksPerSecond_v); }
	_s32 GetTotalMiliSecondsInt32() const { return _s32(Tick / detail::TicksPerMiliSecond_v); }
	_s32 GetTotalMicroSecondsInt32() const { return _s32(Tick); }

	void AddMicroSecond(int _microsec) { Tick += _microsec * detail::TicksPerMicroSecond_v; }
	void AddMiliSecond(int _milisec) { Tick += _milisec * detail::TicksPerMiliSecond_v; }
	void AddSecond(int _sec) { Tick += _sec * detail::TicksPerSecond_v; }
	void AddMinute(int _minute) { Tick += _minute * detail::TicksPerMinute_v;  }
	void AddHour(int _hour) { Tick += _hour * detail::TicksPerHour_v; }
	void AddDay(int _day) { Tick += _day * detail::TicksPerDay_v; }
	
	int GetDay() const { return int(Tick / detail::TicksPerDay_v); }
	int GetHour() const { return (Tick / detail::TicksPerHour_v) % detail::MaxHour_v; }
	int GetMinute() const { return (Tick / detail::TicksPerMinute_v) % detail::MaxMinute_v; }
	int GetSecond() const { return (Tick / detail::TicksPerSecond_v) % detail::MaxSecond_v; }
	int GetMiliSecond() const { return (Tick / detail::TicksPerMiliSecond_v) % detail::MaxMiliSecond_v; }
	int GetMicroSecond() const { return Tick % detail::MaxMicroSecond_v; }

	TimeSpan operator-(const TimeSpan& _other) const { return { Tick - _other.Tick }; }
	TimeSpan operator+(const TimeSpan& _other) const { return { Tick + _other.Tick }; }
	TimeSpan& operator+=(const TimeSpan& _other) { Tick += _other.Tick; return *this; }
	TimeSpan& operator-=(const TimeSpan& _other) { Tick -= _other.Tick; return *this; }
	bool operator>(const TimeSpan& _other) const { return Tick > _other.Tick; }
	bool operator<(const TimeSpan& _other) const { return Tick < _other.Tick; }
	bool operator>=(const TimeSpan& _other) const { return Tick >= _other.Tick; }
	bool operator<=(const TimeSpan& _other) const { return Tick <= _other.Tick; }
	bool operator==(const TimeSpan& _other) const { return Tick == _other.Tick; }

	static TimeSpan FromMicroSeocnd(_s64 _v) { return TimeSpan{ _v }; }
	static TimeSpan FromMiliSeocnd(_s64 _v) { return TimeSpan{ _v * detail::TicksPerMiliSecond_v }; }
	static TimeSpan FromSecond(_s64 _v) { return TimeSpan{ _v * detail::TicksPerSecond_v }; }
	static TimeSpan FromMinute(_s64 _v) { return TimeSpan{ _v * detail::TicksPerMinute_v }; }
	static TimeSpan FromHour(_s64 _v) { return TimeSpan{ _v * detail::TicksPerHour_v }; }
	static TimeSpan FromDay(_s64 _v) { return TimeSpan{ _v * detail::TicksPerDay_v }; }

	

	_s64 Tick{};
};

// 음수 시간을 다룰 수 있는 구조체 (초단위 자릿수, 마이크로초단위 정밀도)
// Cocos2d-x의 프레임 시간단위가 float 타입의 초단위라서 추가함
struct TimeSpanF
{
	TimeSpanF(float _sec = 0) : Second(_sec) {}

	void SetZero() { Second = 0; }

	float GetTotalDays() const { return Second / detail::SecondsPerDayf_v; }
	float GetTotalHours() const { return Second / detail::SecondsPerHourf_v; }
	float GetTotalMinutes() const { return Second / detail::SecondsPerMinutef_v; }
	float GetTotalSeconds() const { return Second / detail::SecondsPerSecondf_v; }
	float GetTotalMiliSeconds() const { return Second / detail::SecondsPerMiliSecondf_v; }
	float GetTotalMicroSeconds() const { return Second / detail::SecondsPerMicroSecondf_v; }

	TimeSpanF operator-(const TimeSpanF& _other) const { return { Second - _other.Second }; }
	TimeSpanF operator+(const TimeSpanF& _other) const { return { Second + _other.Second }; }
	TimeSpanF& operator+=(const TimeSpanF& _other) { Second += _other.Second; return *this; }
	TimeSpanF& operator-=(const TimeSpanF& _other) { Second -= _other.Second; return *this; }
	bool operator>(const TimeSpanF& _other) const { return Second > _other.Second; }
	bool operator<(const TimeSpanF& _other) const { return Second < _other.Second; }
	bool operator>=(const TimeSpanF& _other) const { return Second >= _other.Second; }
	bool operator<=(const TimeSpanF& _other) const { return Second <= _other.Second; }
	bool operator==(const TimeSpanF& _other) const { return Second == _other.Second; }

	static TimeSpanF FromMicroSeocnd(float _microsec) { return TimeSpanF{ _microsec * detail::SecondsPerMicroSecondf_v }; }
	static TimeSpanF FromMiliSeocnd(float _milisec) { return TimeSpanF{ _milisec * detail::SecondsPerMiliSecondf_v }; }
	static TimeSpanF FromSecond(float _sec) { return TimeSpanF{ _sec }; }
	static TimeSpanF FromMinute(float _minute) { return TimeSpanF{ _minute * detail::SecondsPerMinutef_v }; }
	static TimeSpanF FromHour(float _hour) { return TimeSpanF{ _hour * detail::SecondsPerHourf_v }; }
	static TimeSpanF FromDay(float _day) { return TimeSpanF{ _day * detail::SecondsPerDayf_v }; }

	float Second;
};


/*=====================================================================================
								 DateTime
 =====================================================================================*/

#define DATETIME_PARSE_ERROR_INVALID_ARGUMENT_NULL							10000			// 인자가 nullptr로 전달된 경우
#define DATETIME_PARSE_ERROR_DUPLICATE_FORMAT_TOKEN							10002			// yyy-mm-y와 같이 중복된 토큰(y)가 존재하는 경우
#define DATETIME_PARSE_ERROR_INVALID_DATE_FORMAT							10003			// 올바르지 않은 Format인 경우
#define DATETIME_PARSE_ERROR_FORMAT_AND_DATESTRING_DELIMITER_MISMATCH		10004			// Format과 DateString의 구분자가 서로 일치하지 않는 경우
#define DATETIME_PARSE_ERROR_FORMAT_AND_DATESTRING_SIZE_NOT_EQUAL			10005			// 구분자로 분리된 Format과 DateString이 서로 크기가 일치하지 않는 경우
#define DATETIME_PARSE_ERROR_NOT_SUPPORTED_TOKEN							10006			// 타임존, AP, PM과 같이 시간계산에 관련된 토큰이 아닌 경우 파싱지원을 안하도록 함.
#define DATETIME_PARSE_ERROR_NOT_IMPLEMENTED_TOKEN							10007			// 문자열 일, 월의 숫자 변환(Monday -> 1)은 아직 구현안함. 쓸데 없을 듯해서..
#define DATETIME_PARSE_ERROR_INVALID_DATESTRING_TOKEN						10008			// fmt: yyyy-mm, dateString: 2023-99일때, 99월은 있을 수가 없으므로
#define DATETIME_PARSE_ERROR_AMBIGUOUS_DATESTRING_TOKEN						10009			// 1) AMPM 정보가 주어지지 않고 h와 hh에 해당하는 정보를 얻을려고하는 경우


class String;
class DateTime
{
public: // constructors
	DateTime() = default;
	DateTime(_s64 _tick) : Tick(_tick) {}

public: // public non-static
	// 특정 타입유닛에 해당하는 전체시간 얻기
	_s64 GetTotalDays() const { return Tick / detail::TicksPerDay_v; }
	_s64 GetTotalHours() const { return Tick / detail::TicksPerHour_v; }
	_s64 GetTotalMinutes() const { return Tick / detail::TicksPerMinute_v; }
	_s64 GetTotalSeconds() const { return Tick / detail::TicksPerSecond_v; }
	_s64 GetTotalMiliSeconds() const { return Tick / detail::TicksPerMiliSecond_v; }
	_s64 GetTotalMicroSeconds() const { return Tick; }

	// 타입유닛별로 시간 얻기
	int GetYear() const { return GetDatePart(DatePart::Year); }
	int GetMonth() const { return GetDatePart(DatePart::Month); }
	int GetDay() const { return GetDatePart(DatePart::Day); }
	int GetHour() const { return GetTotalHours() % detail::MaxHour_v; }
	int GetMinute() const { return GetTotalMinutes() % detail::MaxMinute_v; }
	int GetSecond() const { return GetTotalSeconds() % detail::MaxSecond_v; }
	int GetMiliSecond() const { return GetTotalMiliSeconds() % detail::MaxMiliSecond_v; }
	int GetMicroSecond() const { return GetTotalMicroSeconds() % detail::MaxMicroSecond_v; }

	
	DayOfWeek GetDayOfWeek() const { return DayOfWeek(GetTotalDays() % 7); }		// 무슨 요일인지
	MonthOfYear GetMonthEnum() const { return MonthOfYear(GetMonth() - 1); }		// 몇달인지
	int GetDayOfYear() const { return GetDatePart(DatePart::DayOfYear); }			// 일년기준으로 몇일인지
	int GetMaxDayOfMonth() const { return GetDatePart(DatePart::MaxDayOfMonth); }	// 이번달이 최대 몇일인지
	AMPM GetAMPM() const { return (GetHour() / 12) > 0 ? AMPM::PM : AMPM::AM; }		// 오전인지 오후인지

	// 타입 변환
	DateAndTime ToDateAndTime() const;
	Date ToDate() const;
	Time ToTime() const;

	// 시간 연산
	DateTime AddYear(_s32 _year);
	DateTime AddMonth(_s32 _month);
	DateTime AddDay(_s32 _day);
	DateTime AddHour(_s64 _hour);
	DateTime AddMinute(_s64 _minute);
	DateTime AddSecond(_s64 _second);
	DateTime AddMiliSecond(_s64 _miliSecond);
	DateTime AddMicroSecond(_s64 _microSecond);
	DateTime AddDateTime(const DateTime& _other);

	DateTime SubtractYear(_s32 _year);
	DateTime SubtractMonth(_s32 _month);
	DateTime SubtractDay(_s32 _day);
	DateTime SubtractHour(_s64 _hour);
	DateTime SubtractMinute(_s64 _minute);
	DateTime SubtractSecond(_s64 _second);
	DateTime SubtractMiliSecond(_s64 _miliSecond);
	DateTime SubtractMicroSecond(_s64 _microSecond);
	DateTime SubtractDateTime(const DateTime& _other);

	int Compare(const DateTime& _other) const { return Comparator<_s64>()(Tick, _other.Tick); }
	TimeSpan Diff(const DateTime& _other) const;

	DateTime operator-(const DateTime& _other) const;
	DateTime operator+(const DateTime& _other) const;
	DateTime& operator-=(const DateTime& _other);
	DateTime& operator+=(const DateTime& _other);
	DateTime operator-(const TimeSpan& _other) const;
	DateTime operator+(const TimeSpan& _other) const;
	DateTime& operator-=(const TimeSpan& _other);
	DateTime& operator+=(const TimeSpan& _other);
	DateTime operator-(const DateAndTime& _other) const;
	DateTime operator+(const DateAndTime& _other) const;
	DateTime& operator-=(const DateAndTime& _other);
	DateTime& operator+=(const DateAndTime& _other);
	bool operator>(const DateTime& _other) const;
	bool operator<(const DateTime& _other) const;
	bool operator>=(const DateTime& _other) const;
	bool operator<=(const DateTime& _other) const;
	bool operator==(const DateTime& _other) const;
	bool operator>(const TimeSpan& _other) const;
	bool operator<(const TimeSpan& _other) const;
	bool operator>=(const TimeSpan& _other) const;
	bool operator<=(const TimeSpan& _other) const;
	bool operator==(const TimeSpan& _other) const;
	bool operator>(const DateAndTime& _other) const;
	bool operator<(const DateAndTime& _other) const;
	bool operator>=(const DateAndTime& _other) const;
	bool operator<=(const DateAndTime& _other) const;
	bool operator==(const DateAndTime& _other) const;

	int FormatBuffered(const char* _fmt, char* _pBuff, int _capacity) const;
	String Format(const char* _fmt) const;
	String FormatMysqlTime() const { return Format("yyyy-MM-dd HH:mm:ss.ffffff"); }

private: // private static
	static Tuple<int, int, int, int, int> GetYearsFromDays(int _days);		// 단위 년도별로 일수를 가져옴
	static int GetDatePart(_s64 _tick, DatePart _part);
	
private: // private non-static
	int GetDatePart(const DatePart _part) const { return GetDatePart(Tick, _part); }
	void ReflectFormat(const DateAndTime& _time, String& _ret, char _token, int _count) const;
	void ReflectFormatBuffered(const DateAndTime& _time, char* _pBuff, int _capacity, int& _pos, char _token, int _count) const;
	static void CheckOverFlow(_u64 _tick);
public: // public static
	static DateTime Now(TimeStandard _timeStandard = TimeStandard::Local);
	static _s32 TimeZoneBiasMinute();
	static bool IsLeapYear(int _year);

	// TODO: TryParse 유닛 테스트
	static bool TryParse(DateTime& _parsed, const char* _fmt, const String& _dateString) { return TryParse(_parsed, _fmt, StringUtil::Length(_fmt), _dateString.Source(), _dateString.Length()); }
	static bool TryParse(DateTime& _parsed, const char* _fmt, const char* _dateString) { return TryParse(_parsed, _fmt, StringUtil::Length(_fmt), _dateString, StringUtil::Length(_dateString)); }
	static bool TryParse(DateTime& _parsed, const char* _fmt, int _fmtLen, const char* _dateString, int _dateStringLen);
	static DateTime FromUnixTime(double _unixTimestamp, TimeStandard _timeStandard = TimeStandard::Local);	// 초단위 스탬프를 DateTime으로 변환
	static int LastError() { return ms_tlsiLastError; }
	static const char* LastErrorMessage();
	static Vector<DateFormat_t> ParseFormat(const String& _fmt, IN_OPT Vector<String>* _delimiters = nullptr) { return ParseFormat(_fmt.Source(), _fmt.Length(), _delimiters); }
	static Vector<DateFormat_t> ParseFormat(const char* _fmt, int _fmtLen, IN_OPT Vector<String>* _delimiters = nullptr);

	static const char* GetAbbreviationWeekendName(DayOfWeek _week) {
		return ms_szWeekAbbrevName[int(_week)];
	}

	static const char* GetFullWeekendName(DayOfWeek _week) {
		return ms_szWeekFullName[int(_week)];
	}

	static const char* GetAbbreviationMonthName(MonthOfYear _month) {
		return ms_szMonthAbbrevName[int(_month)];
	}

	static const char* GetFullMonthName(MonthOfYear _month) {
		return ms_szMonthFullName[int(_month)];
	}

	static const char* GetFullAMPMName(AMPM _ampm);
	static const char* GetAbbreviationAMPMName(AMPM _ampm);

	_u64 Tick{};

	static const char* ms_szWeekAbbrevName[];
	static const char* ms_szWeekFullName[];
	static const char* ms_szMonthAbbrevName[];
	static const char* ms_szMonthFullName[];
	static const char* ms_szAMPMAbbrevName[];
	static const char* ms_szAMPMFullName[];
	static thread_local int ms_tlsiLastError;
			
	friend struct DateAndTime;
	friend struct Date;
	friend struct Time;
};



/*=====================================================================================
									StopWatch
 =====================================================================================*/

enum class StopWatchMode
{
	System,
	HighResolution
};

template <StopWatchMode Mode>
struct StopWatch;

// 정밀도: 밀리초, 반환은 마이크로초 단위로 함
template <>
struct StopWatch<StopWatchMode::System>
{
	_u64	 Start();			// 시작 지점 등록
	TimeSpan Stop();			// 시작 틱을 정지 틱으로 초기화
	TimeSpan GetElapsed();		// 시작 후 경과 시간

	int StartTick{};
};

// 정밀도: 마이크로초
template <>
struct StopWatch<StopWatchMode::HighResolution>
{
	StopWatch();

	_u64	 Start();
	TimeSpan Stop();	
	TimeSpan GetElapsed();		// 시작 후 경과 시간

	_u64 Precision;
	_u64 Frequency{};
	_u64 StartCounter{};
};

using StopWatchHR = StopWatch<StopWatchMode::HighResolution>;
using StopWatchSys = StopWatch<StopWatchMode::System>;

JC_ENUM_CLASS_BIT_OPERATION_OVERLOADING(TimeCounterAttribute)
enum class TimeCounterAttribute
{
	None				= 0b0000,
	FirstCheckFire		= 0b0001,	// 처음 ElapsedSeconds가 실행되면 무조건 true 반환
	FirstElpasedFire	= 0b0010,	// 처음 ElapsedSeconds만큼 시간이 경과되었을때만 true를 반환 (이후로 무조건 DontFire플래그가 켜져서 무조건 false반환)
	TimeOverReset		= 0b0100,	// Elapsed만큼 경과시 0으로 초기화
	DontFire			= 0b1000	// ElapsedSeconds함수가 무조건 false를 반환(Fire 특성들이 있더라도 이게 우선 적용됨)
};

struct TimeCounterBase
{
	TimeCounterBase() : AttributeFlag(TimeCounterAttribute::None) {}
	TimeCounterBase(TimeCounterAttribute _flag) : AttributeFlag(_flag) {}

	BitFlag<TimeCounterAttribute> AttributeFlag;
};

struct TimeCounter : TimeCounterBase
{
	TimeCounter() : Elapsed(0) {}
	TimeCounter(TimeCounterAttribute _attribute)
		: TimeCounterBase(_attribute)
		, Elapsed(0)
	{}

	TimeCounter(const TimeCounter& _other) {
		AttributeFlag.Value = _other.AttributeFlag.Value;
		Elapsed.Tick = _other.Elapsed.Tick;
	}

	TimeCounter& operator=(const TimeCounter& _other) {
		AttributeFlag.Value = _other.AttributeFlag.Value;
		Elapsed.Tick = _other.Elapsed.Tick;
		return *this;
	}

	bool ElapsedSeconds(float _seconds);
	bool ElapsedMiliSeconds(float _miliSec) { return ElapsedSeconds(_miliSec / 1000); }

	TimeSpan Elapsed;
};

struct TimeCounterF : TimeCounterBase
{
	TimeCounterF() : Elapsed(0) {}
	TimeCounterF(TimeCounterAttribute _attribute)
		: TimeCounterBase(_attribute)
		, Elapsed(0)
	{}

	TimeCounterF(const TimeCounterF& _other) {
		AttributeFlag.Value = _other.AttributeFlag.Value;
		Elapsed.Second = _other.Elapsed.Second;
	}

	TimeCounterF& operator=(const TimeCounterF& _other) {
		AttributeFlag.Value = _other.AttributeFlag.Value;
		Elapsed.Second = _other.Elapsed.Second;
		return *this;
	}

	bool ElapsedSeconds(float _seconds);
	bool ElapsedMiliSeconds(float _miliSec) { return ElapsedSeconds(_miliSec / 1000); }

	TimeSpanF Elapsed;
};

NS_END

