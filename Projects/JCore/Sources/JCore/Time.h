/*
	작성자 : 윤정도
	간단한 시간을 다룰 수 있는 기능을 추가합니다.
	스톱워치 기능

	시간 관련지식이 전무해서
	C# DateTime 클래스의 년도 계산 로직 일부를 참고하였습니다.
	구현다보니 좀 자세히 알게됨 ㅋ
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/Tuple.h>
#include <JCore/Comparator.h>
#include <JCore/Natvis/NatvisFloat.h>
#include <JCore/Container/Vector.h>

NS_JC_BEGIN
	NS_DETAIL_BEGIN

	constexpr Int64 MaxDay_v			= 365;
	constexpr Int64 MaxHour_v			= 24;
	constexpr Int64 MaxMinute_v			= 60;
	constexpr Int64 MaxSecond_v			= 60;
	constexpr Int64 MaxMiliSecond_v		= 1000;
	constexpr Int64 MaxMicroSecond_v	= 1000;
		
	// 1틱당 1마이크로초
	constexpr Int64 TicksPerMicroSecond_v	= 1;										// 마이크로초당 몇 틱인지 	1틱당					1마이크로초
	constexpr Int64 TicksPerMiliSecond_v	= TicksPerMicroSecond_v * MaxMicroSecond_v;	// 밀리초당 몇 틱인지		1000틱당					1밀리초
	constexpr Int64 TicksPerSecond_v		= TicksPerMiliSecond_v * MaxMiliSecond_v;		// 초당 몇 틱인지			1000000틱당				1초
	constexpr Int64 TicksPerMinute_v		= TicksPerSecond_v * MaxSecond_v;				// 1분당 몇 틱인지		1000000 * 60			1분
	constexpr Int64 TicksPerHour_v		    = TicksPerMinute_v * MaxMinute_v;				// 1시간당 몇 틱인지		1000000 * 60 * 60		1시간
	constexpr Int64 TicksPerDay_v			= TicksPerHour_v * MaxHour_v;					// 1일당 몇 틱인지		1000000 * 60 * 60 * 24	1일

	constexpr int DaysPer1Years_v = MaxDay_v;
	constexpr int DaysPer4Years_v = MaxDay_v * 4 + 1;				// 4년이 몇일인지 : 1461일
	constexpr int DaysPer100Years_v = DaysPer4Years_v * 25 - 1;		// 100년이 몇일인지 : 36524일
	constexpr int DaysPer400Years_v = DaysPer100Years_v * 4 + 1;	// 400년이 몇일인지 : 146097일
		
	constexpr int DaysUntilMonth365_v[13] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
	constexpr int DaysUntilMonth366_v[13] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

	constexpr int DaysForMonth365_v[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	constexpr int DaysForMonth366_v[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	constexpr Int64 Ticks10000Years_v = DaysPer400Years_v * 25 * TicksPerDay_v;



	/*
		After Christ 1970년도까지의 시간을 구한다음에 해봤는데 오차가 생김;
		아래 처럼 일 수의 소수점 단위까지 들고가면 안된다.
		constexpr long double EpochTimeBeginAC = long double(DaysPer400Years_v * 1969) / 400; // 일 수의 소수점 단위는 버려야함
	*/
		
	// [AD 0001년 1월 1일 ~ 1969년 12월 31일까지의 마이크로초 계산]
	constexpr Int64 ADBegin_v	  = (DaysPer400Years_v * 1969) / 400;	// Epoch 시간 1970년 1월 1일까지의 일 수
	constexpr Int64 ADBeginTick_v = ADBegin_v * TicksPerDay_v;			// Epoch 시간 1970년 1월 1일까지의 마이크로초

	NS_DETAIL_END


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
enum class DateFormat
{
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
	// fffffff  // 나노초 100의 자리까지 표시 -> 마이크로초 까지만 지원하므로 제외한다.
};

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
	Date(Int32 year, Int32 month, Int32 day);
	Date(const Date& other) : Year(other.Year), Month(other.Month), Day(other.Day) {}

	virtual void AddYear(Int32 years);
	virtual void AddMonth(Int32 months);
	virtual void AddDay(Int32 days);
	virtual void AddDate(const Date& other);

	virtual void SubtractYear(Int32 years);
	virtual void SubtractMonth(Int32 years);
	virtual void SubtractDay(Int32 years);
	virtual void SubtractDate(const Date& other);

	Int32 GetYear() const { return Int32(Year); }
	Int32 GetMonth() const { return Int32(Month); }
	Int32 GetDay() const { return Int32(Day); }

	int Compare(const Date& other) const { return Comparator<Int64>()(ToTick(), other.ToTick()); }

	Date operator-(const Date& other) const;
	Date operator+(const Date& other) const;
	Date& operator-=(const Date& other);
	Date& operator+=(const Date& other);
	bool operator>(const Date& other);
	bool operator<(const Date& other);
	bool operator>=(const Date& other);
	bool operator<=(const Date& other);
	bool operator==(const Date& other);

	Int64 ToTick() const;
protected:
	Int16	Year;
	Int8	Month;
	Int8	Day;

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
	Time(Int32 hour, Int32 minute, Int32 second, Int32 miliSecond, Int32 microSecond);
	Time(const Time& other) : Hour(other.Hour), Minute(other.Minute), Second(other.Second), MiliSecond(other.MiliSecond), MicroSecond(other.MicroSecond) {}

	virtual void AddHour(Int64 hours);
	virtual void AddMinute(Int64 minutes);
	virtual void AddSecond(Int64 seconds);
	virtual void AddMiliSecond(Int64 miliSeconds);
	virtual void AddMicroSecond(Int64 microSeconds, TimeUnit timeUnit = TimeUnit::MicroSecond);
	virtual void AddTime(const Time& other);

	virtual void SubtractHour(Int64 hours);
	virtual void SubtractMinute(Int64 minutes);
	virtual void SubtractSecond(Int64 seconds);
	virtual void SubtractMiliSecond(Int64 miliSeconds);
	virtual void SubtractMicroSecond(Int64 microSeconds, TimeUnit timeUnit = TimeUnit::MicroSecond);
	virtual void SubtractTime(const Time& other);

	Int32 GetHour() const { return Hour; }
	Int32 GetMinute() const { return Minute; }
	Int32 GetSecond() const { return Second; }
	Int32 GetMiliSecond() const { return MiliSecond; }
	Int32 GetMicroSecond() const { return MicroSecond; }

	int Compare(const Time& other) const { return Comparator<Int64>()(ToTick(), other.ToTick()); }

	Time operator-(const Time& other) const;
	Time operator+(const Time& other) const;
	Time& operator-=(const Time& other);
	Time& operator+=(const Time& other);
	bool operator>(const Time& other) const;
	bool operator<(const Time& other) const;
	bool operator>=(const Time& other) const;
	bool operator<=(const Time& other) const;
	bool operator==(const Time& other) const;


	Int64 ToTick() const;
protected:
	Int8 Hour;
	Int8 Minute;
	Int8 Second;
	Int16 MiliSecond;
	Int16 MicroSecond;

	friend class DateTime;
};


/*=====================================================================================
									DateAndTime
 =====================================================================================*/
class DateTime;
struct DateAndTime : Date, Time {
	DateAndTime() = default;
	DateAndTime(Int32 year, Int32 month, Int32 day, Int32 hour, Int32 minute, Int32 second, Int32 miliSecond, Int32 microSecond)
		: Date(year, month, day), Time(hour, minute, second, miliSecond, microSecond)
	{}

	void AddYear(Int32 years) override;				// Date::AddYear와 동일
	void AddMonth(Int32 months) override;			// Date::AddMonth와 동일
	void AddDay(Int32 days) override;				
	void AddHour(Int64 hours) override;				
	void AddMinute(Int64 minutes) override;			
	void AddSecond(Int64 seconds) override;			
	void AddMiliSecond(Int64 miliSeconds) override;	
	void AddMicroSecond(Int64 microSeconds, TimeUnit timeUnit = TimeUnit::MicroSecond) override;
	void AddDate(const Date& other) override;
	void AddTime(const Time& other) override;
	void AddDateAndTime(const DateAndTime& other);

	void SubtractYear(Int32 years) override;
	void SubtractMonth(Int32 years) override;
	void SubtractDay(Int32 years) override;
	void SubtractHour(Int64 hours) override;
	void SubtractMinute(Int64 minutes) override;
	void SubtractSecond(Int64 seconds) override;
	void SubtractMiliSecond(Int64 miliSeconds) override;
	void SubtractMicroSecond(Int64 microSeconds, TimeUnit timeUnit = TimeUnit::MicroSecond) override;
	void SubtractDate(const Date& other) override;
	void SubtractTime(const Time& other) override;
	void SubtractDateAndTime(const DateAndTime& other);

	int Compare(const DateAndTime& other) const { return Comparator<Int64>()(ToTick(), other.ToTick()); }

	DateAndTime operator-(const DateAndTime& other) const;
	DateAndTime operator+(const DateAndTime& other) const;
	DateAndTime& operator-=(const DateAndTime& other);
	DateAndTime& operator+=(const DateAndTime& other);

	DateAndTime operator-(const DateTime& other) const;
	DateAndTime operator+(const DateTime& other) const;
	DateAndTime& operator-=(const DateTime& other);
	DateAndTime& operator+=(const DateTime& other);

	bool operator>(const DateAndTime& other);
	bool operator<(const DateAndTime& other);
	bool operator>=(const DateAndTime& other);
	bool operator<=(const DateAndTime& other);
	bool operator==(const DateAndTime& other);

	bool operator>(const DateTime& other) const;
	bool operator<(const DateTime& other) const;
	bool operator>=(const DateTime& other) const;
	bool operator<=(const DateTime& other) const;
	bool operator==(const DateTime& other) const;

	DateTime ToDateTime() const;
	Int64 ToTick() const;
};



/*=====================================================================================
								 TimeSpan
 =====================================================================================*/


// 음수 시간을 다룰 수 있는 구조체
struct TimeSpan
{
	TimeSpan(Int64 tick = 0) : Tick(tick) {}
	TimeSpan(Int32 days, Int64 hours, Int64 minutes, Int64 seconds, Int64 miliSeconds, Int64 microSeconds);

	double GetTotalDays() const { return (double)Tick / Detail::TicksPerDay_v; }
	double GetTotalHours() const { return (double)Tick / Detail::TicksPerHour_v; }
	double GetTotalMinutes() const { return (double)Tick / Detail::TicksPerMinute_v; }
	double GetTotalSeconds() const { return (double)Tick / Detail::TicksPerSecond_v; }
	double GetTotalMiliSeconds() const { return (double)Tick / Detail::TicksPerMiliSecond_v; }
	double GetTotalMicroSeconds() const { return (double)Tick; }

	Int64U GetTotalDaysInt() const { return Tick / Detail::TicksPerDay_v; }
	Int64U GetTotalHoursInt() const { return Tick / Detail::TicksPerHour_v; }
	Int64U GetTotalMinutesInt() const { return Tick / Detail::TicksPerMinute_v; }
	Int64U GetTotalSecondsInt() const { return Tick / Detail::TicksPerSecond_v; }
	Int64U GetTotalMiliSecondsInt() const { return Tick / Detail::TicksPerMiliSecond_v; }
	Int64U GetTotalMicroSecondsInt() const { return Tick; }

	int GetDay() const { return int(Tick / Detail::TicksPerDay_v); }
	int GetHour() const { return (Tick / Detail::TicksPerHour_v) % Detail::MaxHour_v; }
	int GetMinute() const { return (Tick / Detail::TicksPerMinute_v) % Detail::MaxMinute_v; }
	int GetSecond() const { return (Tick / Detail::TicksPerSecond_v) % Detail::MaxSecond_v; }
	int GetMiliSecond() const { return (Tick / Detail::TicksPerMiliSecond_v) % Detail::MaxMiliSecond_v; }
	int GetMicroSecond() const { return Tick % Detail::MaxMicroSecond_v; }

	TimeSpan operator-(const TimeSpan& other) const { return { Tick - other.Tick }; }
	TimeSpan operator+(const TimeSpan& other) const { return { Tick + other.Tick }; }
	TimeSpan& operator+=(const TimeSpan& other) { Tick += other.Tick; return *this; }
	TimeSpan& operator-=(const TimeSpan& other) { Tick -= other.Tick; return *this; }
	bool operator>(const TimeSpan& other) const { return Tick > other.Tick; }
	bool operator<(const TimeSpan& other) const { return Tick < other.Tick; }
	bool operator>=(const TimeSpan& other) const { return Tick >= other.Tick; }
	bool operator<=(const TimeSpan& other) const { return Tick <= other.Tick; }
	bool operator==(const TimeSpan& other) const { return Tick == other.Tick; }

	Int64 Tick{};
};


/*=====================================================================================
								 DateTime
 =====================================================================================*/

#define DATETIME_PARSE_ERROR_INVALID_ARGUMENT_NULL							10000			// 인자가 nullptr로 전달된 경우
#define DATETIME_PARSE_ERROR_INVALID_ARGUMENT_FORMAT						10001			// 올바르지 않은 Format
#define DATETIME_PARSE_ERROR_DUPLICATE_FORMAT_TOKEN							10002			// yyy-mm-y와 같이 중복된 토큰(y)가 존재하는 경우
#define DATETIME_PARSE_ERROR_INVALID_DATE_FORMAT							10003
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
	DateTime(Int64 tick) : m_Tick(tick) {}

public: // public non-static
	// 특정 타입유닛에 해당하는 전체시간 얻기
	Int64 GetTick() const { return m_Tick; }
	Int64 GetTotalDays() const { return m_Tick / Detail::TicksPerDay_v; }
	Int64 GetTotalHours() const { return m_Tick / Detail::TicksPerHour_v; }
	Int64 GetTotalMinutes() const { return m_Tick / Detail::TicksPerMinute_v; }
	Int64 GetTotalSeconds() const { return m_Tick / Detail::TicksPerSecond_v; }
	Int64 GetTotalMiliSeconds() const { return m_Tick / Detail::TicksPerMiliSecond_v; }
	Int64 GetTotalMicroSeconds() const { return m_Tick; }

	// 타입유닛별로 시간 얻기
	int GetYear() const { return GetDatePart(DatePart::Year); }
	int GetMonth() const { return GetDatePart(DatePart::Month); }
	int GetDay() const { return GetDatePart(DatePart::Day); }
	int GetHour() const { return GetTotalHours() % Detail::MaxHour_v; }
	int GetMinute() const { return GetTotalMinutes() % Detail::MaxMinute_v; }
	int GetSecond() const { return GetTotalSeconds() % Detail::MaxSecond_v; }
	int GetMiliSecond() const { return GetTotalMiliSeconds() % Detail::MaxMiliSecond_v; }
	int GetMicroSecond() const { return GetTotalMicroSeconds() % Detail::MaxMicroSecond_v; }

	
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
	DateTime AddYear(Int32 year);
	DateTime AddMonth(Int32 month);
	DateTime AddDay(Int32 day);
	DateTime AddHour(Int64 hour);
	DateTime AddMinute(Int64 minute);
	DateTime AddSecond(Int64 second);
	DateTime AddMiliSecond(Int64 miliSecond);
	DateTime AddMicroSecond(Int64 microSecond);
	DateTime AddDateTime(const DateTime& other);

	DateTime SubtractYear(Int32 year);
	DateTime SubtractMonth(Int32 month);
	DateTime SubtractDay(Int32 day);
	DateTime SubtractHour(Int64 hour);
	DateTime SubtractMinute(Int64 minute);
	DateTime SubtractSecond(Int64 second);
	DateTime SubtractMiliSecond(Int64 miliSecond);
	DateTime SubtractMicroSecond(Int64 microSecond);
	DateTime SubtractDateTime(const DateTime& other);

	int Compare(const DateTime& other) const { return Comparator<Int64>()(m_Tick, other.m_Tick); }
	TimeSpan Diff(const DateTime& other) const;

	DateTime operator-(const DateTime& other) const;
	DateTime operator+(const DateTime& other) const;
	DateTime& operator-=(const DateTime& other);
	DateTime& operator+=(const DateTime& other);
	DateTime operator-(const TimeSpan& other) const;
	DateTime operator+(const TimeSpan& other) const;
	DateTime& operator-=(const TimeSpan& other);
	DateTime& operator+=(const TimeSpan& other);
	DateTime operator-(const DateAndTime& other) const;
	DateTime operator+(const DateAndTime& other) const;
	DateTime& operator-=(const DateAndTime& other);
	DateTime& operator+=(const DateAndTime& other);
	bool operator>(const DateTime& other) const;
	bool operator<(const DateTime& other) const;
	bool operator>=(const DateTime& other) const;
	bool operator<=(const DateTime& other) const;
	bool operator==(const DateTime& other) const;
	bool operator>(const TimeSpan& other) const;
	bool operator<(const TimeSpan& other) const;
	bool operator>=(const TimeSpan& other) const;
	bool operator<=(const TimeSpan& other) const;
	bool operator==(const TimeSpan& other) const;
	bool operator>(const DateAndTime& other) const;
	bool operator<(const DateAndTime& other) const;
	bool operator>=(const DateAndTime& other) const;
	bool operator<=(const DateAndTime& other) const;
	bool operator==(const DateAndTime& other) const;

	String Format(const char* fmt) const;
	String FormatMysqlTime() const { return Format("yyyy-MM-dd HH:mm:ss.ffffff"); }

private: // private static
	static Tuple<int, int, int, int, int> GetYearsFromDays(int days);		// 단위 년도별로 일수를 가져옴
	static int GetDatePart(const Int64 tick, const DatePart part);
	
private: // private non-static
	int GetDatePart(const DatePart part) const { return GetDatePart(m_Tick, part); }
	void ReflectFormat(const DateAndTime& time, String& ret, char token, int count) const;
	static void CheckOverFlow(Int64U tick);
public: // public static
	static DateTime Now(TimeStandard timeStandard = TimeStandard::Local);
	static Int32 TimeZoneBiasMinute();
	static bool IsLeapYear(int year);

	// TODO: TryParse 유닛 테스트
	static bool TryParse(DateTime& parsed, const char* fmt, const String& dateString) { return TryParse(parsed, fmt, StringUtil::Length(fmt), dateString.Source(), dateString.Length()); }
	static bool TryParse(DateTime& parsed, const char* fmt, const char* dateString) { return TryParse(parsed, fmt, StringUtil::Length(fmt), dateString, StringUtil::Length(dateString)); }
	static bool TryParse(DateTime& parsed, const char* fmt, int fmtLen, const char* dateString, int dateStringLen);
	static DateTime FromUnixTime(double unixTimestamp, TimeStandard timeStandard = TimeStandard::Local);	// 초단위 스탬프를 DateTime으로 변환
	static int LastError() { return ms_tlsiLastError; }
	static Vector<DateFormat> ParseFormat(const String& fmt, JCORE_IN_OPT Vector<String>* delimiters = nullptr) { return ParseFormat(fmt.Source(), fmt.Length(), delimiters); }
	static Vector<DateFormat> ParseFormat(const char* fmt, int fmtLen, JCORE_IN_OPT Vector<String>* delimiters = nullptr);

	static const char* GetAbbreviationWeekendName(DayOfWeek week) {
		return ms_szWeekAbbrevName[int(week)];
	}

	static const char* GetFullWeekendName(DayOfWeek week) {
		return ms_szWeekFullName[int(week)];
	}

	static const char* GetAbbreviationMonthName(MonthOfYear month) {
		return ms_szMonthAbbrevName[int(month)];
	}

	static const char* GetFullMonthName(MonthOfYear month) {
		return ms_szMonthFullName[int(month)];
	}

	static const char* GetFullAMPMName(AMPM ampm);
	static const char* GetAbbreviationAMPMName(AMPM ampm);
private:
	Int64U m_Tick{};

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
	Int64U	 Start();			// 시작 지점 등록
	TimeSpan StopReset();		// 시작 틱을 정지 틱으로 초기화
	TimeSpan StopContinue();	// 시작 틱을 초기화 하지 않고 차이를 반환

	int StartTick{};
};

// 정밀도: 마이크로초
template <>
struct StopWatch<StopWatchMode::HighResolution>
{
	StopWatch();

	Int64U	 Start();
	TimeSpan StopReset();	
	TimeSpan StopContinue();

	Int64U Precision;
	Int64U Frequency{};
	Int64U StartCounter{};
};



NS_JC_END

