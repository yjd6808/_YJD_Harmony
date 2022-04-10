/*
	�ۼ��� : ������
	������ �ð��� �ٷ� �� �ִ� ����� �߰��մϴ�.
	�����ġ ���

	�ð� ���������� �����ؼ�
	C# DateTime Ŭ������ �⵵ ��� ���� �Ϻθ� �����Ͽ����ϴ�.
	�����ٺ��� �� �ڼ��� �˰Ե� ��
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
		
	// 1ƽ�� 1����ũ����
	constexpr Int64 TicksPerMicroSecond	= 1;										// ����ũ���ʴ� �� ƽ���� 	1ƽ��					1����ũ����
	constexpr Int64 TicksPerMiliSecond	= TicksPerMicroSecond * MaxMicroSecond_v;	// �и��ʴ� �� ƽ����		1000ƽ��					1�и���
	constexpr Int64 TicksPerSecond		= TicksPerMiliSecond * MaxMiliSecond_v;		// �ʴ� �� ƽ����			1000000ƽ��				1��
	constexpr Int64 TicksPerMinute		= TicksPerSecond * MaxSecond_v;				// 1�д� �� ƽ����		1000000 * 60			1��
	constexpr Int64 TicksPerHour		= TicksPerMinute * MaxMinute_v;				// 1�ð��� �� ƽ����		1000000 * 60 * 60		1�ð�
	constexpr Int64 TicksPerDay			= TicksPerHour * MaxHour_v;					// 1�ϴ� �� ƽ����		1000000 * 60 * 60 * 24	1��

	constexpr int DaysPer1Years_v = MaxDay_v;
	constexpr int DaysPer4Years_v = MaxDay_v * 4 + 1;				// 4���� �������� : 1461��
	constexpr int DaysPer100Years_v = DaysPer4Years_v * 25 - 1;		// 100���� �������� : 36524��
	constexpr int DaysPer400Years_v = DaysPer100Years_v * 4 + 1;	// 400���� �������� : 146097��
		
	constexpr int DaysUntilMonth365[13] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
	constexpr int DaysUntilMonth366[13] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

	constexpr int DaysForMonth365[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	constexpr int DaysForMonth366[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	constexpr Int64 Ticks10000Years = DaysPer400Years_v * 25 * TicksPerDay;



	/*
		After Christ 1970�⵵������ �ð��� ���Ѵ����� �غôµ� ������ ����;
		�Ʒ� ó�� �� ���� �Ҽ��� �������� ����� �ȵȴ�.
		constexpr long double EpochTimeBeginAC = long double(DaysPer400Years_v * 1969) / 400; // �� ���� �Ҽ��� ������ ��������
	*/
		
	// [AD 0001�� 1�� 1�� ~ 1969�� 12�� 31�ϱ����� ����ũ���� ���]
	constexpr Int64 ADBegin	 = (DaysPer400Years_v * 1969) / 400;	// Epoch �ð� 1970�� 1�� 1�ϱ����� �� ��
	constexpr Int64 ADBeginTick = ADBegin * TicksPerDay;			// Epoch �ð� 1970�� 1�� 1�ϱ����� ����ũ����
}


enum class DatePart
{
	Year,			// ��⵵����				1 ~ 9999
	Month,			// 1���߿� �������		1 ~ 12
	Day,			// ����޿� ��������		1 ~ (28 or 29 or 30 or 31)
	DayOfYear,		// 1���߿� ���° ������	1 ~ (355 or 366)
	MaxDayOfMonth	// �̹��� �ִ� ����		(28 or 29 or 30 or 31)
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
	Local,			// ���� ��ǻ�� �ý��� ���� �ð�
	Universal		// UTC ���ؽð�
};

// https://www.c-sharpcorner.com/blogs/date-and-time-format-in-c-sharp-programming1
enum class DateFormat
{
	d,			// �� ǥ�� -> 1 ~ 31
	dd,			// �� ǥ�� -> 01 ~ 31
	ddd,		// ���� ��� �̸�-> Mon, Tue, Wed 
	dddd,		// ���� ��ü �̸� -> Monday, Tuesday, Wednesday
	h,			// 12�ð� ���� �ð� -> 2
	hh,			// 12�ð� ���� �ð� -> 02
	H,			// 24�ð� ���� �ð� -> 2, 23
	HH,			// 24�ð� ���� �ð� -> 02, 23
	m,			// �� -> 5
	mm,			// �� -> 05
	M,			// �� -> 1
	MM,			// �� -> 01
	MMM,		// �� ��� �̸� -> Dec, Sep
	MMMM,		// �� ��ü �̸� -> December
	s,			// �� -> 5��
	ss,			// �� -> 05��
	t,			// ���� ���� -> A, P
	tt,			// ���� ���� -> AM, PM
	y,			// 2�ڸ��⵵ -> 2009 -> 9
	yy,			// 2�ڸ��⵵ -> 2009 -> 09
	yyy,		// 4�ڸ��⵵ -> 2022 -> 2022
	yyyy,		// 4�ڸ��⵵ -> 2022 -> 2022
	K,			// Ÿ���� �ð� (�ð�:��) -> +09:00
	z,			// Ÿ���� �ð� (�ð�) -> +9
	zz,			// Ÿ���� �ð� (�ð�) -> +09
	zzz,		// Ÿ���� �ð� (�ð�:��) -> +09:00
	f,			// �и��� 100�� �ڸ����� ǥ��		10.123456 -> 1
	ff,			// �и��� 10�� �ڸ����� ǥ��		10.123456 -> 12
	fff,		// �и��� 1�� �ڸ����� ǥ��			10.123456 -> 123
	ffff,		// ����ũ���� 100�� �ڸ����� ǥ��	10.123456 -> 1234
	fffff,		// ����ũ���� 10�� �ڸ����� ǥ��	10.123456 -> 12345
	ffffff,		// ����ũ���� 1�� �ڸ����� ǥ��		10.123456 -> 123456
	// fffffff  // ������ 100�� �ڸ����� ǥ�� -> ����ũ���� ������ �����ϹǷ� �����Ѵ�.
};

/* =================================================================================== 
	�ð� ����
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
	�ð� ��� �� ����

	Date�� Time, DateAndTime�� DateTimedml ���� ���� ���� �������� ��¥, �ð�, ƽ�� ������ ���� ��� ���ܸ� �����Ƿ�
	������ �ð� ���̸� ����ϰ� ���� ��쿡�� DateTime�� Diff �Լ��� ����Ͽ� TimeSpan ����ü�� ���� Ȯ���� ��
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

	void AddYear(Int32 years) override;				// Date::AddYear�� ����
	void AddMonth(Int32 months) override;			// Date::AddMonth�� ����
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

// ���� �ð��� �ٷ� �� �ִ� ����ü
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
	// Ư�� Ÿ�����ֿ� �ش��ϴ� ��ü�ð� ���
	inline Int64 GetTick() const { return m_Tick; }
	inline Int64 GetTotalDays() const { return m_Tick / Detail::TicksPerDay; }
	inline Int64 GetTotalHours() const { return m_Tick / Detail::TicksPerHour; }
	inline Int64 GetTotalMinutes() const { return m_Tick / Detail::TicksPerMinute; }
	inline Int64 GetTotalSeconds() const { return m_Tick / Detail::TicksPerSecond; }
	inline Int64 GetTotalMiliSeconds() const { return m_Tick / Detail::TicksPerMiliSecond; }
	inline Int64 GetTotalMicroSeconds() const { return m_Tick; }

	// Ư�� Ÿ�����ֺ��� �ð� ���
	inline int GetYear() const { return GetDatePart(DatePart::Year); }
	inline int GetMonth() const { return GetDatePart(DatePart::Month); }
	inline int GetDay() const { return GetDatePart(DatePart::Day); }
	inline int GetHour() const { return GetTotalHours() % Detail::MaxHour_v; }
	inline int GetMinute() const { return GetTotalMinutes() % Detail::MaxMinute_v; }
	inline int GetSecond() const { return GetTotalSeconds() % Detail::MaxSecond_v; }
	inline int GetMiliSecond() const { return GetTotalMiliSeconds() % Detail::MaxMiliSecond_v; }
	inline int GetMicroSecond() const { return GetTotalMicroSeconds() % Detail::MaxMicroSecond_v; }

			
	inline DayOfWeek GetDayOfWeek() const { return DayOfWeek(GetTotalDays() % 7); }			// ���� ��������
	inline MonthOfYear GetMonthEnum() const { return MonthOfYear(GetMonth() - 1); }			// �������
	inline int GetDayOfYear() const { return GetDatePart(DatePart::DayOfYear); }			// �ϳ�������� ��������
	inline int GetMaxDayOfMonth() const { return GetDatePart(DatePart::MaxDayOfMonth); }	// �̹����� �ִ� ��������
	inline AMPM GetAMPM() const { return (GetHour() / 12) > 0 ? AMPM::PM : AMPM::AM; }		// �������� ��������

	// Ÿ�� ��ȯ
	inline DateAndTime ToDateAndTime() const;
	inline Date ToDate() const;
	inline Time ToTime() const;

	// �ð� ����
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
	static Tuple<int, int, int, int, int> GetYearsFromDays(int days);		// ���� �⵵���� �ϼ��� ������
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





