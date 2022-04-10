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

namespace JCore {
namespace Detail {
	constexpr Int64 MaxDay_v			= 365;
	constexpr Int64 MaxHour_v			= 24;
	constexpr Int64 MaxMinute_v			= 60;
	constexpr Int64 MaxSecond_v			= 60;
	constexpr Int64 MaxMiliSecond_v		= 1000;
	constexpr Int64 MaxMicroSecond_v	= 1000;
		
	// 1틱당 1마이크로초
	constexpr Int64 TicksPerMicroSecond	= 1;										// 마이크로초당 몇 틱인지 	1틱당					1마이크로초
	constexpr Int64 TicksPerMiliSecond	= TicksPerMicroSecond * MaxMicroSecond_v;	// 밀리초당 몇 틱인지		1000틱당					1밀리초
	constexpr Int64 TicksPerSecond		= TicksPerMiliSecond * MaxMiliSecond_v;		// 초당 몇 틱인지			1000000틱당				1초
	constexpr Int64 TicksPerMinute		= TicksPerSecond * MaxSecond_v;				// 1분당 몇 틱인지		1000000 * 60			1분
	constexpr Int64 TicksPerHour		= TicksPerMinute * MaxMinute_v;				// 1시간당 몇 틱인지		1000000 * 60 * 60		1시간
	constexpr Int64 TicksPerDay			= TicksPerHour * MaxHour_v;					// 1일당 몇 틱인지		1000000 * 60 * 60 * 24	1일

	constexpr int DaysPer1Years_v = MaxDay_v;
	constexpr int DaysPer4Years_v = MaxDay_v * 4 + 1;				// 4년이 몇일인지 : 1461일
	constexpr int DaysPer100Years_v = DaysPer4Years_v * 25 - 1;		// 100년이 몇일인지 : 36524일
	constexpr int DaysPer400Years_v = DaysPer100Years_v * 4 + 1;	// 400년이 몇일인지 : 146097일
		
	constexpr int DaysUntilMonth365[13] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
	constexpr int DaysUntilMonth366[13] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

	constexpr int DaysForMonth365[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	constexpr int DaysForMonth366[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	constexpr Int64 Ticks10000Years = DaysPer400Years_v * 25 * TicksPerDay;



	/*
		After Christ 1970년도까지의 시간을 구한다음에 해봤는데 오차가 생김;
		아래 처럼 일 수의 소수점 단위까지 들고가면 안된다.
		constexpr long double EpochTimeBeginAC = long double(DaysPer400Years_v * 1969) / 400; // 일 수의 소수점 단위는 버려야함
	*/
		
	// [AD 0001년 1월 1일 ~ 1969년 12월 31일까지의 마이크로초 계산]
	constexpr Int64 ADBegin	 = (DaysPer400Years_v * 1969) / 400;	// Epoch 시간 1970년 1월 1일까지의 일 수
	constexpr Int64 ADBeginTick = ADBegin * TicksPerDay;			// Epoch 시간 1970년 1월 1일까지의 마이크로초
}


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
	PM
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
	Date() = delete;
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

	inline Int32 GetYear() const { return Int32(Year); }
	inline Int32 GetMonth() const { return Int32(Month); }
	inline Int32 GetDay() const { return Int32(Day); }

	inline int Compare(const Date& other) const { return Comparator<Int64>()(ToTick(), other.ToTick()); }

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
	Int16	Year{};
	Int8	Month{};
	Int8	Day{};

	friend class DateTime;
};

struct Time
{
public:
	Time() = delete;
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

	inline Int32 GetHour() const { return Hour; }
	inline Int32 GetMinute() const { return Minute; }
	inline Int32 GetSecond() const { return Second; }
	inline Int32 GetMiliSecond() const { return MiliSecond; }
	inline Int32 GetMicroSecond() const { return MicroSecond; }

	inline int Compare(const Time& other) const { return Comparator<Int64>()(ToTick(), other.ToTick()); }

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
	Int8 Hour{};
	Int8 Minute{};
	Int8 Second{};
	Int16 MiliSecond{};
	Int16 MicroSecond{};

	friend class DateTime;
};

class DateTime;
struct DateAndTime : Date, Time {
	DateAndTime() = delete;
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

	inline int Compare(const DateAndTime& other) const { return Comparator<Int64>()(ToTick(), other.ToTick()); }

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

// 음수 시간을 다룰 수 있는 구조체
struct TimeSpan
{
	TimeSpan(Int64 tick = 0) : Tick(tick) {}
	TimeSpan(Int32 days, Int64 hours, Int64 minutes, Int64 seconds, Int64 miliSeconds, Int64 microSeconds);

	inline double GetTotalDays() const { return (double)Tick / Detail::TicksPerDay; }
	inline double GetTotalHours() const { return (double)Tick / Detail::TicksPerHour; }
	inline double GetTotalMinutes() const { return (double)Tick / Detail::TicksPerMinute; }
	inline double GetTotalSeconds() const { return (double)Tick / Detail::TicksPerSecond; }
	inline double GetTotalMiliSeconds() const { return (double)Tick / Detail::TicksPerMiliSecond; }
	inline double GetTotalMicroSeconds() const { return (double)Tick; }

	inline int GetDay() const { return int(Tick / Detail::TicksPerDay); }
	inline int GetHour() const { return (Tick / Detail::TicksPerHour) % Detail::MaxHour_v; }
	inline int GetMinute() const { return (Tick / Detail::TicksPerMinute) % Detail::MaxMinute_v; }
	inline int GetSecond() const { return (Tick / Detail::TicksPerSecond) % Detail::MaxSecond_v; }
	inline int GetMiliSecond() const { return (Tick / Detail::TicksPerMiliSecond) % Detail::MaxMiliSecond_v; }
	inline int GetMicroSecond() const { return Tick % Detail::MaxMicroSecond_v; }

	inline TimeSpan operator-(const TimeSpan& other) const { return { Tick - other.Tick }; }
	inline TimeSpan operator+(const TimeSpan& other) const { return { Tick + other.Tick }; }
	inline TimeSpan& operator+=(const TimeSpan& other) { Tick += other.Tick; return *this; }
	inline TimeSpan& operator-=(const TimeSpan& other) { Tick -= other.Tick; return *this; }
	inline bool operator>(const TimeSpan& other) const { return Tick > other.Tick; }
	inline bool operator<(const TimeSpan& other) const { return Tick < other.Tick; }
	inline bool operator>=(const TimeSpan& other) const { return Tick >= other.Tick; }
	inline bool operator<=(const TimeSpan& other) const { return Tick <= other.Tick; }
	inline bool operator==(const TimeSpan& other) const { return Tick == other.Tick; }

	Int64 Tick{};
};

class String;
class DateTime
{
public: // constructors
	DateTime() = default;
	DateTime(Int64 tick) : m_Tick(tick) {}

public: // public non-static
	// 특정 타입유닛에 해당하는 전체시간 얻기
	inline Int64 GetTick() const { return m_Tick; }
	inline Int64 GetTotalDays() const { return m_Tick / Detail::TicksPerDay; }
	inline Int64 GetTotalHours() const { return m_Tick / Detail::TicksPerHour; }
	inline Int64 GetTotalMinutes() const { return m_Tick / Detail::TicksPerMinute; }
	inline Int64 GetTotalSeconds() const { return m_Tick / Detail::TicksPerSecond; }
	inline Int64 GetTotalMiliSeconds() const { return m_Tick / Detail::TicksPerMiliSecond; }
	inline Int64 GetTotalMicroSeconds() const { return m_Tick; }

	// 특정 타입유닛별로 시간 얻기
	inline int GetYear() const { return GetDatePart(DatePart::Year); }
	inline int GetMonth() const { return GetDatePart(DatePart::Month); }
	inline int GetDay() const { return GetDatePart(DatePart::Day); }
	inline int GetHour() const { return GetTotalHours() % Detail::MaxHour_v; }
	inline int GetMinute() const { return GetTotalMinutes() % Detail::MaxMinute_v; }
	inline int GetSecond() const { return GetTotalSeconds() % Detail::MaxSecond_v; }
	inline int GetMiliSecond() const { return GetTotalMiliSeconds() % Detail::MaxMiliSecond_v; }
	inline int GetMicroSecond() const { return GetTotalMicroSeconds() % Detail::MaxMicroSecond_v; }

			
	inline DayOfWeek GetDayOfWeek() const { return DayOfWeek(GetTotalDays() % 7); }			// 무슨 요일인지
	inline MonthOfYear GetMonthEnum() const { return MonthOfYear(GetMonth() - 1); }			// 몇달인지
	inline int GetDayOfYear() const { return GetDatePart(DatePart::DayOfYear); }			// 일년기준으로 몇일인지
	inline int GetMaxDayOfMonth() const { return GetDatePart(DatePart::MaxDayOfMonth); }	// 이번달이 최대 몇일인지
	inline AMPM GetAMPM() const { return (GetHour() / 12) > 0 ? AMPM::PM : AMPM::AM; }		// 오전인지 오후인지

	// 타입 변환
	inline DateAndTime ToDateAndTime() const;
	inline Date ToDate() const;
	inline Time ToTime() const;

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

	inline int Compare(const DateTime& other) const { return Comparator<Int64>()(m_Tick, other.m_Tick); }
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
	String FormatMysqlTime() const { return Format("yyyy-MM-dd hh:mm:ss.ffffff"); };

private: // private static
	static Tuple<int, int, int, int, int> GetYearsFromDays(int days);		// 단위 년도별로 일수를 가져옴
	static int GetDatePart(const Int64 tick, const DatePart part);
private: // private non-static
	inline int GetDatePart(const DatePart part) const { return DateTime::GetDatePart(m_Tick, part); }
	void ReflectFormat(const DateAndTime& time, String& ret, char token, int count) const;
	void CheckOverFlow() const;
public: // public static
	static DateTime Now(TimeStandard timeStandard = TimeStandard::Local);
	static Int32 TimeZoneBiasMinute();
	static bool IsLeapYear(int year);

	inline static const char* GetAbbreviationWeekendName(DayOfWeek week) {
		return ms_szWeekAbbrevName[int(week)];
	}

	inline static const char* GetFullWeekendName(DayOfWeek week) {
		return ms_szWeekFullName[int(week)];
	}

	inline static const char* GetAbbreviationMonthName(MonthOfYear month) {
		return ms_szMonthAbbrevName[int(month)];
	}

	inline static const char* GetFullMonthName(MonthOfYear month) {
		return ms_szMonthFullName[int(month)];
	}

	inline static const char* GetFullAMPMName(AMPM ampm) {
		return ms_szAMPMFullName[int(ampm)];
	}

	inline static const char* GetAbbreviationAMPMName(AMPM ampm) {
		return ms_szAMPMAbbrevName[int(ampm)];
	}
private:
	Int64U m_Tick{};

	static const char* ms_szWeekAbbrevName[];
	static const char* ms_szWeekFullName[];
	static const char* ms_szMonthAbbrevName[];
	static const char* ms_szMonthFullName[];
	static const char* ms_szAMPMAbbrevName[];
	static const char* ms_szAMPMFullName[];
			
	friend struct DateAndTime;
	friend struct Date;
	friend struct Time;
};

} // namespace JCore





