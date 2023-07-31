/*
	작성자 : 윤정도
	간단한 시간을 다룰 수 있는 기능을 추가합니다.
	스톱워치 기능
*/

#include <JCore/Core.h>
#include <JCore/Time.h>
#include <JCore/Math.h>
#include <JCore/Ascii.h>
#include <JCore/Exception.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Primitives/String.h>
#include <JCore/Primitives/StaticString.h>

#include <chrono>
#include <timezoneapi.h>

using namespace std;

NS_JC_BEGIN

	USING_NS_DETAIL;

/*=====================================================================================
									Date
=====================================================================================*/

Date::Date(Int32 year, Int32 month, Int32 day) {
	if (year < 1 || year > 9999)
		throw InvalidArgumentException("년은 1 ~ 9999년까지 가능합니다.");
	if (month < 1 || month > 12)
		throw InvalidArgumentException("월은 1 ~ 12까지 가능합니다.");
	if (day < 1 || day > 31)
		throw InvalidArgumentException("일은 1 ~ 31까지 가능합니다.");

	Year = year;
	Month = month;
	Day = day;

	AddMonth(0);
}

void Date::AddYear(Int32 years) {
	AddMonth(years * 12);
}

void Date::AddMonth(Int32 months) {
	if (months < 0) {
		SubtractMonth(months);
		return;
	}

	Int32 iTempMonth = Month;
	Int32 iTempYear = Year;

	iTempMonth += months;

	if (iTempMonth > 12) {
		const int iAddedYear = (iTempMonth - 1) / 12;
		iTempYear += iAddedYear;
		iTempMonth -= iAddedYear * 12;
	}

	if (iTempYear >= 10000) {
		throw OverFlowException("Date 달 덧셈 수행중 오류가 발생하였습니다. 1만년을 넘길 수 없습니다.");
	}

	Month = iTempMonth;
	Year = iTempYear;

	// 만약 1월 31일날 13개월을 더했다고 했을 때 
	// 달 단위만 변경해버리면 2월 31일이 되어버린다. (2월은 마지막일이 윤년일 경우 29일 평년일 경우 28일 임)
	// 그래서 윤년/평년에 따라 해당 달의 최대일 수를 초과하는 경우 최대 일수로 변경해줘야함
	if (DateTime::IsLeapYear(Year)) {
		Day = Day > DaysForMonth366_v[Month - 1] ? DaysForMonth366_v[Month - 1] : Day;
	} else {
		Day = Day > DaysForMonth365_v[Month - 1] ? DaysForMonth365_v[Month - 1] : Day;
	}
}

void Date::AddDay(Int32 day) {
	const Int64 tick = ToTick() + day * TicksPerDay_v;

	if (tick < 0) {
		throw UnderFlowException("Date 일 덧셈 수행중 오류가 발생하였습니다. 일 연산 수행결과로 음수가 나오면 안됩니다.");
	}

	const Date date = DateTime(tick).ToDate();

	Year = date.Year;
	Month = date.Month;
	Day = date.Day;
}

void Date::AddDate(const Date& other) {
	AddDay(other.Day);
	AddMonth(other.Year * 12 + other.Month);
}


void Date::SubtractYear(Int32 years) {
	SubtractYear(years * 12);
}

void Date::SubtractMonth(Int32 months) {
	if (months < 0) {
		AddMonth(Math::Abs(months));
		return;
	}

	const Int32 iSubtractedYear = months / 12;
	Int32 iTempYear = Year;
	Int32 iTempMonth = Month;
	iTempYear -= iSubtractedYear;
	months -= iSubtractedYear * 12;

	if (iTempMonth - months >= 1) {
		iTempMonth -= months;
	} else {
		iTempMonth += 12;
		iTempYear -= 1;
		iTempMonth -= months;
	}

	if (iTempYear <= 0 || iTempMonth <= 0) {
		throw UnderFlowException("Date 달 빨셈 수행중 오류가 발생하였습니다. 년도와 달이 0이하가 될 수 없습니다.");
	}

	Year = static_cast<Int16>(iTempYear);
	Month = static_cast<Int8>(iTempMonth);

	if (DateTime::IsLeapYear(Year)) {
		Day = Day > DaysForMonth366_v[Month - 1] ? DaysForMonth366_v[Month - 1] : Day;
	} else {
		Day = Day > DaysForMonth365_v[Month - 1] ? DaysForMonth365_v[Month - 1] : Day;
	}
}

void Date::SubtractDay(Int32 days) {
	if (days < 0) {
		AddDay(Math::Abs(days));
		return;
	}

	AddDay(days * -1);

}

void Date::SubtractDate(const Date& other) {
	SubtractDay(other.Day);
	SubtractMonth(other.Year * 12 + other.Month);
}


Date Date::operator-(const Date& other) const {
	Date temp(*this);
	temp.SubtractDate(other);
	return temp;
}

Date Date::operator+(const Date& other) const {
	Date temp(*this);
	temp.AddDate(other);
	return temp;
}

Date& Date::operator-=(const Date& other) {
	SubtractDate(other);
	return *this;
}

Date& Date::operator+=(const Date& other) {
	AddDate(other);
	return *this;
}

bool Date::operator>(const Date& other) {
	return Compare(other) > 0;
}

bool Date::operator<(const Date& other) {
	return Compare(other) < 0;
}

bool Date::operator>=(const Date& other) {
	return Compare(other) >= 0;
}

bool Date::operator<=(const Date& other) {
	return Compare(other) <= 0;
}

bool Date::operator==(const Date& other) {
	return Compare(other) == 0;
}

Int64 Date::ToTick() const {
	int iCurrentYear = Year > 0 ? Year - 1 : 0;
	int iTotalDays = 0;

	const int i400Years = iCurrentYear / 400;
	iCurrentYear -= i400Years * 400;

	const int i100Years = iCurrentYear / 100;
	iCurrentYear -= i100Years * 100;

	const int i4Years = iCurrentYear / 4;
	iCurrentYear -= i4Years * 4;

	const int i1Years = iCurrentYear;

	iTotalDays = i400Years * DaysPer400Years_v;
	iTotalDays += i100Years * DaysPer100Years_v;
	iTotalDays += i4Years * DaysPer4Years_v;
	iTotalDays += i1Years * DaysPer1Years_v;

	// 현재 계산된 달, 일은 0일, 0월부터 시작아니라 1일, 1월 부터 시작이기때문에 1씩 빼줘야한다.
	if (DateTime::IsLeapYear(Year)) {
		iTotalDays += DaysUntilMonth366_v[Month - 1] + Day - 1;
	} else {
		iTotalDays += DaysUntilMonth365_v[Month - 1] + Day - 1;
	}

	return iTotalDays * TicksPerDay_v;
}


/*=====================================================================================
										Time
	=====================================================================================*/

Time::Time(Int32 hour, Int32 minute, Int32 second, Int32 miliSecond, Int32 microSecond) {

	if (hour < 0 || hour > 23)
		throw InvalidArgumentException("시간은 0 ~ 23까지만 가능합니다.");
	if (minute < 0 || minute > 59)
		throw InvalidArgumentException("분은 0 ~ 59까지만 가능합니다.");
	if (second < 0 || second > 59)
		throw InvalidArgumentException("초는 0 ~ 59까지만 가능합니다.");
	if (miliSecond < 0 || miliSecond > 999)
		throw InvalidArgumentException("밀리초는 0 ~ 999까지만 가능합니다.");
	if (microSecond < 0 || microSecond > 999)
		throw InvalidArgumentException("마이크로초는 0 ~ 999까지만 가능합니다.");

	Hour = hour;
	Minute = minute;
	Second = second;
	MiliSecond = miliSecond;
	MicroSecond = microSecond;
}

void Time::AddHour(const Int64 hours) {
	AddMicroSecond(hours * TicksPerHour_v, TimeUnit::Hour);
}

void Time::AddMinute(const Int64 minutes) {
	AddMicroSecond(minutes * TicksPerMinute_v, TimeUnit::Minute);
}

void Time::AddSecond(Int64 seconds) {
	AddMicroSecond(seconds * TicksPerSecond_v, TimeUnit::Second);
}

void Time::AddMiliSecond(Int64 miliSeconds) {
	AddMicroSecond(miliSeconds * TicksPerMiliSecond_v, TimeUnit::MiliSecond);
}

void Time::AddMicroSecond(Int64 microSeconds, TimeUnit timeUnit) {
	const Int64 iTick = ToTick() + microSeconds;

	if (iTick < 0) {
		throw UnderFlowException("Time 연산 수행중 오류가 발생하였습니다. 음수가 나오면 안됩니다.");
	}

	const Int64 uiHour = iTick / TicksPerHour_v;
	Hour = uiHour % MaxHour_v;

	if (timeUnit == TimeUnit::Hour)
		return;

	const Int64 uiMinute = iTick / TicksPerMinute_v;
	Minute = uiMinute % MaxMinute_v;

	if (timeUnit == TimeUnit::Minute)
		return;

	const Int64 uiSecond = iTick / TicksPerSecond_v;
	Second = uiSecond % MaxSecond_v;

	if (timeUnit == TimeUnit::Second)
		return;

	const Int64 uiMiliSecond = iTick / TicksPerMiliSecond_v;
	MiliSecond = uiMiliSecond % MaxMiliSecond_v;

	if (timeUnit == TimeUnit::MiliSecond)
		return;


	MicroSecond = iTick % MaxMicroSecond_v;
}

void Time::AddTime(const Time& other) {
	const Int64 iTick = ToTick() + other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("Time 연산 수행중 오류가 발생하였습니다. 음수가 나오면 안됩니다.");
	}

	*this = DateTime(iTick).ToTime();
}

void Time::SubtractHour(const Int64 hours) {
	SubtractMicroSecond(hours * TicksPerHour_v, TimeUnit::Hour);
}

void Time::SubtractMinute(const Int64 minutes) {
	SubtractMicroSecond(minutes * TicksPerMinute_v, TimeUnit::Minute);
}

void Time::SubtractSecond(const Int64 seconds) {
	SubtractMicroSecond(seconds * TicksPerSecond_v, TimeUnit::Second);
}

void Time::SubtractMiliSecond(const Int64 miliSeconds) {
	SubtractMicroSecond(miliSeconds * TicksPerMiliSecond_v, TimeUnit::MiliSecond);
}

void Time::SubtractMicroSecond(Int64 microSeconds, TimeUnit timeUnit) {
	if (microSeconds < 0) {
		AddMicroSecond(Math::Abs(microSeconds));
		return;
	}

	AddMicroSecond(microSeconds * -1);
}

void Time::SubtractTime(const Time& other) {
	const Int64 iTick = ToTick() - other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("Time 연산 수행중 오류가 발생하였습니다. 음수가 나오면 안됩니다.");
	}

	*this = DateTime(iTick).ToTime();
}


Time Time::operator-(const Time& other) const {
	Time temp(*this);
	temp.SubtractTime(other);
	return temp;
}

Time Time::operator+(const Time& other) const {
	Time temp(*this);
	temp.AddTime(other);
	return temp;
}

Time& Time::operator-=(const Time& other) {
	SubtractTime(other);
	return *this;
}

Time& Time::operator+=(const Time& other) {
	AddTime(other);
	return *this;
}

bool Time::operator>(const Time& other) const {
	return Compare(other) > 0;
}

bool Time::operator<(const Time& other) const {
	return Compare(other) < 0;
}

bool Time::operator>=(const Time& other) const {
	return Compare(other) >= 0;
}

bool Time::operator<=(const Time& other) const {
	return Compare(other) <= 0;
}

bool Time::operator==(const Time& other) const {
	return Compare(other) == 0;
}

Int64 Time::ToTick() const {
	Int64 uiTotalTick = 0;
	uiTotalTick += Hour * TicksPerHour_v;
	uiTotalTick += Minute * TicksPerMinute_v;
	uiTotalTick += Second * TicksPerSecond_v;
	uiTotalTick += MiliSecond * TicksPerMiliSecond_v;
	uiTotalTick += MicroSecond * TicksPerMicroSecond_v;
	return uiTotalTick;
}


/*=====================================================================================
									DateAndTime
=====================================================================================*/

DateTime DateAndTime::ToDateTime() const {
	return DateTime(ToTick());
}

Int64 DateAndTime::ToTick() const {
	return Time::ToTick() + Date::ToTick();
}

void DateAndTime::AddYear(Int32 years) {
	Date::AddMonth(years * 12);
}

void DateAndTime::AddMonth(Int32 months) {
	Date::AddMonth(months);
}

void DateAndTime::AddDay(const Int32 days) {
	AddMicroSecond(days * TicksPerDay_v);
}

void DateAndTime::AddHour(const Int64 hours) {
	AddMicroSecond(hours * TicksPerHour_v);
}

void DateAndTime::AddMinute(const Int64 minutes) {
	AddMicroSecond(minutes * TicksPerMinute_v);
}

void DateAndTime::AddSecond(const Int64 seconds) {
	AddMicroSecond(seconds * TicksPerSecond_v);
}

void DateAndTime::AddMiliSecond(const Int64 miliSeconds) {
	AddMicroSecond(miliSeconds * TicksPerMiliSecond_v);
}

void DateAndTime::AddMicroSecond(Int64 microSeconds, TimeUnit timeUnit) {
	const Int64 iTick = ToTick() + microSeconds;

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime 연산 수행중 오류가 발생하였습니다. 음수가 나오면 안됩니다.");
	}

	if (iTick >= Ticks10000Years_v) {
		throw UnderFlowException("DateAndTime 연산 수행중 오류가 발생하였습니다. 1만년 이상이 될 수 없습니다.");
	}
	const DateTime current(iTick);
	*this = current.ToDateAndTime();
}

void DateAndTime::AddDate(const Date& other) {
	const Int64 iTick = ToTick() + other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime 연산 수행중 오류가 발생하였습니다. 음수가 나오면 안됩니다.");
	}

	if (iTick >= Ticks10000Years_v) {
		throw UnderFlowException("DateAndTime 연산 수행중 오류가 발생하였습니다. 1만년 이상이 될 수 없습니다.");
	}
	const DateTime current(iTick);
	*this = current.ToDateAndTime();
}

void DateAndTime::AddTime(const Time& other) {
	const Int64 iTick = ToTick() + other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime 연산 수행중 오류가 발생하였습니다. 음수가 나오면 안됩니다.");
	}

	const DateTime current(iTick);
	*this = current.ToDateAndTime();
}

void DateAndTime::AddDateAndTime(const DateAndTime& other) {
	const Int64 iTick = ToTick() + other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime 연산 수행중 오류가 발생하였습니다. 음수가 나오면 안됩니다.");
	}

	if (iTick >= Ticks10000Years_v) {
		throw UnderFlowException("DateAndTime 연산 수행중 오류가 발생하였습니다. 1만년 이상이 될 수 없습니다.");
	}

	const DateTime current(iTick);
	*this = current.ToDateAndTime();
}

void DateAndTime::SubtractYear(const Int32 years) {
	Date::SubtractMonth(years * 12);
}

void DateAndTime::SubtractMonth(const Int32 months) {
	Date::SubtractMonth(months);
}

void DateAndTime::SubtractDay(const Int32 days) {
	SubtractMicroSecond(days * TicksPerDay_v);
}

void DateAndTime::SubtractHour(const Int64 hours) {
	SubtractMicroSecond(hours * TicksPerHour_v);
}

void DateAndTime::SubtractMinute(const Int64 minutes) {
	SubtractMicroSecond(minutes * TicksPerMinute_v);
}

void DateAndTime::SubtractSecond(const Int64 seconds) {
	SubtractMicroSecond(seconds * TicksPerSecond_v);
}

void DateAndTime::SubtractMiliSecond(const Int64 miliSeconds) {
	SubtractMicroSecond(miliSeconds * TicksPerMiliSecond_v);
}

void DateAndTime::SubtractMicroSecond(const Int64 microSeconds, TimeUnit timeUnit) {
	const Int64 iTick = ToTick() - microSeconds;

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime 연산 수행중 오류가 발생하였습니다. 음수가 나오면 안됩니다.");
	}
	const DateTime current(iTick);
	*this = current.ToDateAndTime();
}

void DateAndTime::SubtractDate(const Date& other) {
	const Int64 iTick = ToTick() - other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime 연산 수행중 오류가 발생하였습니다. 음수가 나오면 안됩니다.");
	}

	const DateTime current(iTick);
	*this = current.ToDateAndTime();
}

void DateAndTime::SubtractTime(const Time& other) {
	const Int64 iTick = ToTick() - other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime 연산 수행중 오류가 발생하였습니다. 음수가 나오면 안됩니다.");
	}

	const DateTime current(iTick);
	*this = current.ToDateAndTime();
}

void DateAndTime::SubtractDateAndTime(const DateAndTime& other) {
	SubtractMicroSecond(other.ToTick());
}

DateAndTime DateAndTime::operator-(const DateAndTime& other) const {
	const Int64 iTick = ToTick() - other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime 연산 수행중 오류가 발생하였습니다. 음수가 나오면 안됩니다.");
	}

	return DateTime(iTick).ToDateAndTime();
}

DateAndTime DateAndTime::operator+(const DateAndTime& other) const {
	const Int64 iTick = ToTick() + other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime 연산 수행중 오류가 발생하였습니다. 음수가 나오면 안됩니다.");
	}

	return DateTime(iTick).ToDateAndTime();
}

DateAndTime& DateAndTime::operator-=(const DateAndTime& other) {
	SubtractMicroSecond(other.ToTick());
	return *this;
}

DateAndTime& DateAndTime::operator+=(const DateAndTime& other) {
	SubtractMicroSecond(other.ToTick());
	AddDateAndTime(other);
	return *this;
}

DateAndTime DateAndTime::operator-(const DateTime& other) const {
	const Int64 iTick = ToTick() - other.Tick;

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime 연산 수행중 오류가 발생하였습니다. 음수가 나오면 안됩니다.");
	}

	return DateTime(iTick).ToDateAndTime();
}

DateAndTime DateAndTime::operator+(const DateTime& other) const {
	const Int64 iTick = ToTick() + other.Tick;

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime 연산 수행중 오류가 발생하였습니다. 음수가 나오면 안됩니다.");
	}

	return DateTime(iTick).ToDateAndTime();
}

DateAndTime& DateAndTime::operator-=(const DateTime& other) {
	SubtractMicroSecond(other.Tick);
	return *this;
}

DateAndTime& DateAndTime::operator+=(const DateTime& other) {
	AddMicroSecond(other.Tick);
	return *this;
}

bool DateAndTime::operator>(const DateAndTime& other) {
	return Compare(other) > 0;
}

bool DateAndTime::operator<(const DateAndTime& other) {
	return Compare(other) < 0;
}

bool DateAndTime::operator>=(const DateAndTime& other) {
	return Compare(other) >= 0;
}

bool DateAndTime::operator<=(const DateAndTime& other) {
	return Compare(other) <= 0;
}

bool DateAndTime::operator==(const DateAndTime& other) {
	return Compare(other) == 0;
}

bool DateAndTime::operator>(const DateTime& other) const {
	return ToTick() > other.Tick;
}

bool DateAndTime::operator<(const DateTime& other) const {
	return ToTick() < other.Tick;
}

bool DateAndTime::operator>=(const DateTime& other) const {
	return ToTick() >= other.Tick;
}

bool DateAndTime::operator<=(const DateTime& other) const {
	return ToTick() <= other.Tick;
}

bool DateAndTime::operator==(const DateTime& other) const {
	return ToTick() == other.Tick;
}

/*=====================================================================================
									DateTime
	=====================================================================================*/

const char* DateTime::ms_szWeekAbbrevName[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
const char* DateTime::ms_szWeekFullName[] = {
	"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
};
const char* DateTime::ms_szMonthAbbrevName[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
const char* DateTime::ms_szMonthFullName[] = {
	"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November",
	"December"
};
const char* DateTime::ms_szAMPMAbbrevName[] = {"A", "P"};
const char* DateTime::ms_szAMPMFullName[] = {"AM", "PM"};
thread_local int DateTime::ms_tlsiLastError;

/*
	키 : char (포맷 시작 앞문자)
	값 : Tuple<char, Format, int>

			char : 포맷 시작 앞문자
			Format : 포맷 이넘 시작 정수
			int : 포매팅 가능한 최대 문자 갯수

	예를들어서 포맷 분석중 y를 연속 세번 만난 경우 Format::y + 2를 해당 포맷 조건을 찾아가도록 한다.
	그리고 y를 연속 5번 만나버린 경우. 즉 해당 포맷 문자에서 가증한 최대 문자 갯수를 초과한 경우는 오류를 뛰우기 위함
*/

HashMap<char, Tuple<char, DateFormat, int>> FormatTokenMap_v =
{
	{'d', {'d', DateFormat::d, 4}},
	{'h', {'h', DateFormat::h, 2}},
	{'H', {'H', DateFormat::H, 2}},
	{'m', {'m', DateFormat::m, 2}},
	{'M', {'M', DateFormat::M, 4}},
	{'s', {'s', DateFormat::s, 2}},
	{'t', {'t', DateFormat::t, 2}},
	{'y', {'y', DateFormat::y, 4}},
	{'K', {'K', DateFormat::K, 1}},
	{'z', {'z', DateFormat::z, 3}},
	{'f', {'f', DateFormat::f, 6}}
};

HashMap<String, DateFormat> DateFormatMap_v =
{
	{ "d", DateFormat::d },
	{ "dd", DateFormat::dd },
	{ "ddd", DateFormat::ddd },
	{ "dddd", DateFormat::dddd },
	{ "h", DateFormat::h },
	{ "hh", DateFormat::hh },
	{ "H", DateFormat::H },
	{ "HH", DateFormat::HH },
	{ "m", DateFormat::m },
	{ "mm", DateFormat::mm },
	{ "M", DateFormat::M },
	{ "MM", DateFormat::MM },
	{ "MMM", DateFormat::MMM },
	{ "MMMM", DateFormat::MMMM },
	{ "s", DateFormat::s },
	{ "ss", DateFormat::ss },
	{ "t", DateFormat::t },
	{ "tt", DateFormat::tt },
	{ "y", DateFormat::y },
	{ "yy", DateFormat::yy },
	{ "yyy", DateFormat::yyy },
	{ "yyyy", DateFormat::yyyy },
	{ "K", DateFormat::K },
	{ "z", DateFormat::z },
	{ "zz", DateFormat::zz },
	{ "zzz", DateFormat::zzz },
	{ "f", DateFormat::f },
	{ "ff", DateFormat::ff },
	{ "fff", DateFormat::fff },
	{ "ffff", DateFormat::ffff },
	{ "fffff", DateFormat::fffff },
	{ "ffffff", DateFormat::ffffff }
};


/*=====================================================================================
	public
	=====================================================================================*/
inline DateAndTime DateTime::ToDateAndTime() const {
	return DateAndTime(static_cast<Int32U>(GetYear()), static_cast<Int32U>(GetMonth()),
	                   static_cast<Int32U>(GetDay()), // Date 정보
	                   static_cast<Int32U>(GetHour()), static_cast<Int32U>(GetMinute()),
	                   static_cast<Int32U>(GetSecond()), static_cast<Int32U>(GetMiliSecond()),
	                   static_cast<Int32U>(GetMicroSecond())); // Time 정보
}

inline Date DateTime::ToDate() const {
	return Date(static_cast<Int32U>(GetYear()), static_cast<Int32U>(GetMonth()), static_cast<Int32U>(GetDay()));
}

inline Time DateTime::ToTime() const {
	return Time(static_cast<Int32U>(GetHour()), static_cast<Int32U>(GetMinute()), static_cast<Int32U>(GetSecond()),
	            static_cast<Int32U>(GetMiliSecond()), static_cast<Int32U>(GetMicroSecond()));
}


// 포맷에 따라 시간 문자열을 얻도록 한다.
// @형식 레퍼런스 : https://www.c-sharpcorner.com/blogs/date-and-time-format-in-c-sharp-programming1
// C# 기준 포맷 형식을 따릅니다.
// O(n)
// 개선 가능성 : template<char[Size]>를 추가해서 리터럴 문자열에 대해서 길이 계산을 할 수 있다.
String DateTime::Format(const char* fmt) const {
	const int iFmtLen = StringUtil::Length(fmt);
	String szRet(iFmtLen * 2);

	int iContinuousCount = 0;
	char cContinuousToken = '\0';

	const DateAndTime currentDateAndTime = ToDateAndTime();

	for (int i = 0; i < iFmtLen; i++) {
		if (FormatTokenMap_v.Exist(fmt[i])) {

			if (cContinuousToken != fmt[i]) {
				// 이전 토큰하고 다른 경우 = 처음 발견한 경우
				ReflectFormat(currentDateAndTime, szRet, cContinuousToken, iContinuousCount);
				cContinuousToken = fmt[i];
				iContinuousCount = 1;
			} else {
				// 이전 토큰하고 일치하고 있는 경우
				iContinuousCount++;
			}
		} else {
			if (IsAlphabat(fmt[i])) {
				throw InvalidArgumentException("토큰 문자외의 알파벳 문자가 포맷문자열에 포함되어 있습니다.");
			}

			// 토큰이 아닌 다른 문자를 발견한 경우
			ReflectFormat(currentDateAndTime, szRet, cContinuousToken, iContinuousCount);
			cContinuousToken = '\0';
			szRet += fmt[i];
			iContinuousCount = 0;
		}
	}

	ReflectFormat(currentDateAndTime, szRet, cContinuousToken, iContinuousCount);

	return szRet;
}

bool DateTime::TryParse(DateTime& parsed, const char* fmt, int fmtLen, const char* dateString, int dateStringLen) {

	if (fmt == nullptr || dateString == nullptr) {
		ms_tlsiLastError = DATETIME_PARSE_ERROR_INVALID_ARGUMENT_NULL;
		return false;
	}

	DateAndTime result;
	Vector<String> vDelimiterList;
	Vector<DateFormat> vFormatList = ParseFormat(fmt, fmtLen, &vDelimiterList);			// 1. dateString과 fmt을 1:1대응 시키기 위함 2. 동일한 포맷 토큰이 포함되어 있는지 (YYYY-mm-YYYY) 이런식으로 중복된 포멧이 있는 경우를 체크하는 용도
	Vector<Pair<int, String>> vDateStringList;	// 기존 인덱스정보도 함께 저장

	// ParseFormat 수행중 오류가 발생한 경우
	if (ms_tlsiLastError != 0) {
		return false;
	}

	// DateFormat이 하나도 없는 경우
	if (vFormatList.Size() == 0) {
		parsed.Tick = 0;
		return true;
	}

	int iDateStringDelimiterPos = 0;

	// 예를들어서
	// fmt = "YYYY===mm-DD 라는 형식이 들어온 경우
	//  => vFormatList = { YYYY, mm, DD }
	//  => vDelimiterList = { ===, - } 와 같이 설정
	// dateString = 2023===43-01과 같이 구분자가 일치해야한다.
	for (int i = 0; i < vDelimiterList.Size(); ++i) {
		const String& szDelimiter = vDelimiterList[i];
		const int iPrevDateStringDelimiterPos = iDateStringDelimiterPos;
		iDateStringDelimiterPos = StringUtil::Find(dateString, dateStringLen, iPrevDateStringDelimiterPos, szDelimiter.Source());

		// 포맷에는 현재 구분자가 있지만 dateString에는 해당 구분자가 없는 경우
		if (iDateStringDelimiterPos == -1) {
			ms_tlsiLastError = DATETIME_PARSE_ERROR_FORMAT_AND_DATESTRING_DELIMITER_MISMATCH;
			return false;
		}

		String szDateString = StringUtil::GetRange(dateString, dateStringLen, iPrevDateStringDelimiterPos, iDateStringDelimiterPos - 1);
		vDateStringList.PushBack({ vDateStringList.Size(), Move(szDateString)});
		iDateStringDelimiterPos += szDelimiter.Length();
	}

	if (iDateStringDelimiterPos < dateStringLen) {
		String szDateString = StringUtil::GetRange(dateString, dateStringLen, iDateStringDelimiterPos, dateStringLen - 1);
		vDateStringList.PushBack({ vDateStringList.Size(), Move(szDateString) });
	}

	if (vDateStringList.Size() != vFormatList.Size()) {
		ms_tlsiLastError = DATETIME_PARSE_ERROR_FORMAT_AND_DATESTRING_SIZE_NOT_EQUAL;
		return false;
	}

	// 잘못된 파싱결과를 방지하기 위해 포맷별로 우선순위 수치를 지정
	// 예를들어 AM, PM에 대한 정보를 우선적으로 얻어야지 d와 dd 포맷에 대해서 올바른 값을 얻을 수 있다.
	static Vector<int> s_DateFormatParsePriority = {
		4, /* d			*/
		4, /* dd		*/
		4, /* ddd		*/
		4, /* dddd		*/
		5, /* h			*/
		5, /* hh		*/
		5, /* H			*/
		5, /* HH		*/
		6, /* m			*/
		6, /* mm		*/
		3, /* M			*/
		3, /* MM		*/
		3, /* MMM		*/
		3, /* MMMM		*/
		7, /* s			*/
		7, /* ss		*/
		1, /* t			*/
		1, /* tt		*/
		2, /* y			*/
		2, /* yy		*/
		2, /* yyy		*/
		2, /* yyyy		*/
		0, /* K			*/
		0, /* z			*/
		0, /* zz		*/
		0, /* zzz		*/
		8, /* f			*/
		8, /* ff		*/
		8, /* fff		*/
		8, /* ffff		*/
		8, /* fffff		*/
		8  /* ffffff	*/
	};


	// vFormatList를 정렬해서 위치가 변경되기 전에 먼저 vDateStringList 정렬해줘야함.
	vDateStringList.Sort([&vFormatList](auto& lhs, auto& rhs) {
		return s_DateFormatParsePriority[int(vFormatList[lhs.Key])] < s_DateFormatParsePriority[int(vFormatList[rhs.Key])];
	});

	vFormatList.Sort([](DateFormat& lhs, DateFormat& rhs) {
		return s_DateFormatParsePriority[int(lhs)] < s_DateFormatParsePriority[int(rhs)];
	});

	AMPM eAMPM = AMPM::None;
	
	for (int i = 0; i < vFormatList.Size(); ++i) {
		const DateFormat eFormatToken = vFormatList[i];
		String& szDateStringToken = vDateStringList[i].Value;

		switch (eFormatToken) {
		case DateFormat::d:
		case DateFormat::dd: {
			const int day = StringUtil::ToNumber<Int32>(szDateStringToken.Source());
			if (day < 1 || day > 31) {
				ms_tlsiLastError = DATETIME_PARSE_ERROR_INVALID_DATESTRING_TOKEN;
				break;
			}

			result.Day = static_cast<Int8>(day);
			break;
		}
		case DateFormat::ddd:
		case DateFormat::dddd: {
			ms_tlsiLastError = DATETIME_PARSE_ERROR_NOT_IMPLEMENTED_TOKEN;
			break;
		}
		case DateFormat::h:
		case DateFormat::hh: {
			int hour = StringUtil::ToNumber<Int32>(szDateStringToken.Source());

			if (eAMPM == AMPM::None) {
				ms_tlsiLastError = DATETIME_PARSE_ERROR_AMBIGUOUS_DATESTRING_TOKEN;
				break;
			}

			if (hour < 0 || hour > 12) {
				ms_tlsiLastError = DATETIME_PARSE_ERROR_INVALID_DATESTRING_TOKEN;
				break;
			}

			if (eAMPM == AMPM::PM) {
				hour += 12;
			}

			result.Hour = static_cast<Int8>(hour);
			break;
		}
		case DateFormat::H:
		case DateFormat::HH: {
			const int hour = StringUtil::ToNumber<Int32>(szDateStringToken.Source());
			if (hour < 0 || hour > 23) {
				ms_tlsiLastError = DATETIME_PARSE_ERROR_INVALID_DATESTRING_TOKEN;
				break;
			}
			result.Hour = static_cast<Int8>(hour);
			break;
		}
		case DateFormat::m:
		case DateFormat::mm: {
			const int minute = StringUtil::ToNumber<Int32>(szDateStringToken.Source());
			if (minute < 0 || minute > 59) {
				ms_tlsiLastError = DATETIME_PARSE_ERROR_INVALID_DATESTRING_TOKEN;
				break;
			}
			result.Minute = static_cast<Int8>(minute);
			break;
		}
		case DateFormat::M:
		case DateFormat::MM: {
			const int month = StringUtil::ToNumber<Int32>(szDateStringToken.Source());
			if (month < 0 || month > 12) {
				ms_tlsiLastError = DATETIME_PARSE_ERROR_INVALID_DATESTRING_TOKEN;
				break;
			}
			result.Month = static_cast<Int8>(month);
			break;
		}
		case DateFormat::MMM:
		case DateFormat::MMMM: {
			ms_tlsiLastError = DATETIME_PARSE_ERROR_NOT_IMPLEMENTED_TOKEN;
			break;
		}
		case DateFormat::s:
		case DateFormat::ss: {
			const int sec = StringUtil::ToNumber<Int32>(szDateStringToken.Source());
			if (sec < 0 || sec > 59) {
				ms_tlsiLastError = DATETIME_PARSE_ERROR_INVALID_DATESTRING_TOKEN;
				break;
			}
			result.Second = static_cast<Int8>(sec);
			break;
		}
		case DateFormat::t: {
			if (szDateStringToken == "A") {
				eAMPM = AMPM::AM;
				break;
			}

			if (szDateStringToken == "P") {
				eAMPM = AMPM::PM;
				break;
			}

			ms_tlsiLastError = DATETIME_PARSE_ERROR_INVALID_DATESTRING_TOKEN;
			break;
		}
		case DateFormat::tt: {
			if (szDateStringToken == "AM") {
				eAMPM = AMPM::AM;
				break;
			}

			if (szDateStringToken == "PM") {
				eAMPM = AMPM::PM;
				break;
			}
			ms_tlsiLastError = DATETIME_PARSE_ERROR_INVALID_DATESTRING_TOKEN;
			break;
		}
		case DateFormat::y:
		case DateFormat::yy: {
			const int year2 = StringUtil::ToNumber<Int32>(szDateStringToken.Source());
			if (year2 < 0 || year2 > 99) {
				ms_tlsiLastError = DATETIME_PARSE_ERROR_INVALID_DATESTRING_TOKEN;
				break;
			}
			result.Year = static_cast<Int16>(year2 + 2000);
			break;
		}
		case DateFormat::yyy:
		case DateFormat::yyyy: {
			const int year4 = StringUtil::ToNumber<Int32>(szDateStringToken.Source());
			if (year4 < 0 || year4 > 9999) {
				ms_tlsiLastError = DATETIME_PARSE_ERROR_INVALID_DATESTRING_TOKEN;
				break;
			}
			result.Year = static_cast<Int16>(year4);
			break;
		}
		case DateFormat::K:
		case DateFormat::z:
		case DateFormat::zz:
		case DateFormat::zzz: {
			ms_tlsiLastError = DATETIME_PARSE_ERROR_NOT_SUPPORTED_TOKEN;
			break;
		}
		case DateFormat::f:
		case DateFormat::ff: 
		case DateFormat::fff: 
		case DateFormat::ffff:
		case DateFormat::fffff:
		case DateFormat::ffffff: {
			// 어떻게 변환하면 좋을까... 케이스를 하나로 통합할 방법을 고민해보자.
			// 0.12 -> 6자리로 만듬 -> 120000 -> 상위 3자리, 하위 3자리 숫자 분리
			// 0.0012 -> 6자리로 만듬 -> 001200 -> 상위 3자리, 하위 3자리 숫자 분리

			// 1) f라면? 상위 3자리 숫자 / 100 * 100, 하위 3자리 0 => 상위 3자리 숫자의 1과 10의 자리수를 없애야하므로 100으로 나눈 후 100을 곱해준다.
			// 2) ff라면? 상위 3자리 숫자 / 10 * 10,  하위 3자리 0 => 상위 3자리 숫자의 1의 자리수를 없애야하므로 10으로 나눈 후 10을 곱해준다.
			// 3) fff라면? 상위 3자리 숫자, 하위 3자리 0
			// 4) ffff라면? 상위 3자리 숫자, 하위 3자리 숫자 / 100 * 100 => 하위 3자리 숫자의 1과 10의 자리수를 없애야하므로 100으로 나눈 후 100을 곱해준다.
			// 생략...

			if (szDateStringToken.Length() >= 7) {
				// 소수점 자릿수가 7자리 이상인 경우는 오류로 판정함
				ms_tlsiLastError = DATETIME_PARSE_ERROR_INVALID_DATESTRING_TOKEN;
				break;
			}

			// 6자리가 될 수 있도록 나머지 자리는 0으로 채워준다.
			while (szDateStringToken.Length() < 6)
				szDateStringToken += '0';

			// 상위 3자리(밀리초), 하위 3자리 숫자(마이크로초)로 분리
			char upper[4]{};
			char lower[4]{};

			Memory::Copy(upper, 3, szDateStringToken.Source(), 3);
			Memory::Copy(lower, 3, szDateStringToken.Source() + 3, 3);
			int iMiliSeconds = StringUtil::ToNumber<Int32>(upper);
			int iMicroSeconds = StringUtil::ToNumber<Int32>(lower);

			switch (eFormatToken) {
			case DateFormat::f: {
				result.MiliSecond = Int16(iMiliSeconds / 100 * 100);
				result.MicroSecond = 0;
				break;
			}
			case DateFormat::ff: {
				result.MiliSecond = Int16(iMiliSeconds / 10 * 10);
				result.MicroSecond = 0;
				break;
			}
			case DateFormat::fff: {
				result.MiliSecond = Int16(iMiliSeconds);
				result.MicroSecond = 0;
				break;
			}
			case DateFormat::ffff: {
				result.MiliSecond = Int16(iMiliSeconds);
				result.MicroSecond = Int16(iMicroSeconds / 100 * 100);
				break;
			}
			case DateFormat::fffff: {
				result.MiliSecond = Int16(iMiliSeconds);
				result.MicroSecond = Int16(iMicroSeconds / 10 * 10);
				break;
			}
			case DateFormat::ffffff: {
				result.MiliSecond = Int16(iMiliSeconds);
				result.MicroSecond = Int16(iMicroSeconds);
				break;
			}
			} // switch End
		} 
		} // switch End

		if (ms_tlsiLastError != 0) {
			return false;
		}
	}

	
	parsed.Tick = result.ToTick();
	ms_tlsiLastError = 0;

	

	return true;
}

DateTime DateTime::FromUnixTime(double unixTimestamp, TimeStandard timeStandard) {
	Int64 uiTick = ADBeginTick_v + Int64(unixTimestamp * 1'000'000); // DateTimne은 마이크로초단위이므로 백만 곱해줌

	// 로컬 시간은 타임존 편차만큼 더해준다.
	if (timeStandard == TimeStandard::Local) {
		const Int32 uiBias = TimeZoneBiasMinute();
		uiTick += (uiBias * -1) * TicksPerMinute_v;
	}

	return uiTick;	
}

const char* DateTime::LastErrorMessage() {
	switch (ms_tlsiLastError) {
	case 0: return "오류 없음";
	case DATETIME_PARSE_ERROR_INVALID_ARGUMENT_NULL:  
		return "인자가 nullptr로 전달된 경우";
	case DATETIME_PARSE_ERROR_DUPLICATE_FORMAT_TOKEN:  
		return "yyy-mm-y와 같이 중복된 토큰(y)가 존재하는 경우";
	case DATETIME_PARSE_ERROR_INVALID_DATE_FORMAT:
		return "올바르지 않은 Format인 경우";
	case DATETIME_PARSE_ERROR_FORMAT_AND_DATESTRING_DELIMITER_MISMATCH:
		return "Format과 DateString의 구분자가 서로 일치하지 않는 경우";
	case DATETIME_PARSE_ERROR_FORMAT_AND_DATESTRING_SIZE_NOT_EQUAL:
		return "구분자로 분리된 Format과 DateString이 서로 크기가 일치하지 않는 경우";
	case DATETIME_PARSE_ERROR_NOT_SUPPORTED_TOKEN:
		return "타임존, AP, PM과 같이 시간계산에 관련된 토큰이 아닌 경우 파싱지원을 안하도록 함.";
	case DATETIME_PARSE_ERROR_NOT_IMPLEMENTED_TOKEN:
		return "문자열 일, 월의 숫자 변환(Monday -> 1)은 아직 구현안함. 쓸데 없을 듯해서..";
	case DATETIME_PARSE_ERROR_INVALID_DATESTRING_TOKEN:
		return "fmt: yyyy-mm, dateString: 2023-99일때, 99월은 있을 수가 없으므로";
	case DATETIME_PARSE_ERROR_AMBIGUOUS_DATESTRING_TOKEN:
		return "1) AMPM 정보가 주어지지 않고 h와 hh에 해당하는 정보를 얻을려고하는 경우";
	default: return "알 수 없는 오류";
	}
}

// YYYY-MM-dd===
// fmtList = { YYYY, MM, dd }
// delimiters = { -, -, === }
// 이런식으로 파싱해주는 함수
Vector<DateFormat> DateTime::ParseFormat(const char* fmt, int fmtLen, JCORE_IN_OPT Vector<String>* delimiters) {

	static auto FnAddDelimiter = [](String& delimiter, Vector<String>* delimiterList)->void {
		if (delimiterList == nullptr) return;
		if (delimiter.Length() == 0) return;
		delimiterList->PushBack(delimiter);
		delimiter.Clear();
	};

	static auto FnAddDateFormat = [](String& dateFormat, Vector<DateFormat>& dateFormatList)->bool {
		if (dateFormat.Length() == 0) 
			return true;

		const DateFormat* pFind = DateFormatMap_v.Find(dateFormat);
		dateFormat.Clear();

		if (pFind == nullptr) {
			ms_tlsiLastError = DATETIME_PARSE_ERROR_INVALID_DATE_FORMAT;
			return false;
		}


		dateFormatList.PushBack(*pFind);
		return true;
	};

	HashMap<char, bool> hTokenDuplicateCheckMap;
	Vector<DateFormat> fmtList;
	String szTempDateFormat;
	String szTempDelimiter;

	for (int i = 0; i < fmtLen; ++i) {

		char fmtToken = fmt[i];

		// 아예 DateFormat 토큰 문자가 아닌 경우
		if (!FormatTokenMap_v.Exist(fmtToken)) {
			if (!FnAddDateFormat(szTempDateFormat, fmtList)) {
				return {};
			}
			szTempDelimiter += fmtToken;
			continue;
		}

		// yyy-mm-y와 같이 중복된 토큰(y)가 존재하는 경우
		if (szTempDateFormat.Length() == 0 && !hTokenDuplicateCheckMap.Insert(fmtToken, true)) {
			ms_tlsiLastError = DATETIME_PARSE_ERROR_DUPLICATE_FORMAT_TOKEN;
			return {};
		}

		FnAddDelimiter(szTempDelimiter, delimiters);
		szTempDateFormat += fmtToken;
	}

	FnAddDelimiter(szTempDelimiter, delimiters);

	if (!FnAddDateFormat(szTempDateFormat, fmtList))
		return {};

	ms_tlsiLastError = 0;
	return fmtList;
}

const char* DateTime::GetFullAMPMName(AMPM ampm) {
	if (ampm == AMPM::None) {
		DebugAssert(false);
		return "";
	}
	return ms_szAMPMFullName[int(ampm)];
}

const char* DateTime::GetAbbreviationAMPMName(AMPM ampm) {
	if (ampm == AMPM::None) {
		DebugAssert(false);
		return "";
	}
	return ms_szAMPMAbbrevName[int(ampm)];
}

// 타임존 시간 편차 얻는 함수
// @참고 : https://docs.microsoft.com/ko-kr/windows/win32/api/timezoneapi/nf-timezoneapi-gettimezoneinformation?redirectedfrom=MSDN (타임존 정보 얻는법)
Int32 DateTime::TimeZoneBiasMinute() {
	TIME_ZONE_INFORMATION timeZoneInformation;
	GetTimeZoneInformation(&timeZoneInformation);
	const Int32 bias = timeZoneInformation.Bias;
	return bias;
}

// 현재시간 얻는 함수
// @참고 : https://stackoverflow.com/questions/15957805/extract-year-month-day-etc-from-stdchronotime-point-in-c (현재시간 얻는 방법)
// @참고 : https://stackoverflow.com/questions/32811729/add-time-duration-to-c-timepoint (시간 연산방법)
DateTime DateTime::Now(TimeStandard timeStandard) {
	const chrono::system_clock::time_point now = chrono::system_clock::now();

	// Unix Epoch UTC 시간을 마이크로초 단위로 구한다.
	//  = 1970년 1월 1일부터 현재까지의 마이크로초
	Int64 epoch = chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()).count();

	// After Christ (0001년 1월 1일 ~ 1969년 12월 31일)까지의 마이크로초를 더해줌으로써
	// 0001년 1월 1일 ~ 현재까지의 After Chirst UTC 시간을 구한다.
	epoch += ADBeginTick_v;

	// 로컬 시간은 타임존 편차만큼 더해준다.
	if (timeStandard == TimeStandard::Local) {
		const Int32 uiBias = TimeZoneBiasMinute();
		epoch += (uiBias * -1) * TicksPerMinute_v;
	}
	return DateTime(epoch);
}



// @윤년 조건 참고 : https://ko.wikipedia.org/wiki/%EC%9C%A4%EB%85%84 
bool DateTime::IsLeapYear(const int year) {
	if (year <= 0) {
		return false;
	}

	if (year % 4 == 0) {
		if (year % 100 == 0) {
			if (year % 400 == 0) {
				return true;
			}
			return false;
		}
		return true;
	}
	return false;
}




DateTime DateTime::AddYear(const Int32 years) {
	AddMonth(years * 12);
	return *this;
}

DateTime DateTime::AddMonth(const Int32 months) {
	DateAndTime current = ToDateAndTime();
	current.AddMonth(months);
	Tick = current.ToTick();
	return *this;
}

DateTime DateTime::AddDay(Int32 day) {
	const Int64U uiTick = Tick + day * TicksPerDay_v;
	CheckOverFlow(uiTick);
	Tick = uiTick;
	return *this;
}

DateTime DateTime::AddHour(Int64 hour) {
	const Int64U uiTick = Tick + hour * TicksPerHour_v;
	CheckOverFlow(uiTick);
	Tick = uiTick;
	return *this;
}

DateTime DateTime::AddMinute(Int64 minute) {
	const Int64U uiTick = Tick + minute * TicksPerMinute_v;
	CheckOverFlow(uiTick);
	Tick = uiTick;
	return *this;
}

DateTime DateTime::AddSecond(Int64 second) {
	const Int64U uiTick = Tick + second * TicksPerSecond_v;
	CheckOverFlow(uiTick);
	Tick = uiTick;
	return *this;
}

DateTime DateTime::AddMiliSecond(Int64 miliSecond) {
	const Int64U uiTick = Tick + miliSecond * TicksPerMiliSecond_v;
	CheckOverFlow(uiTick);
	Tick = uiTick;
	return *this;
}

DateTime DateTime::AddMicroSecond(Int64 microSecond) {
	const Int64U uiTick = Tick + microSecond;
	CheckOverFlow(uiTick);
	Tick = uiTick;
	return *this;
}

DateTime DateTime::AddDateTime(const DateTime& other) {
	AddMicroSecond(other.Tick);
	return *this;
}

DateTime DateTime::SubtractYear(Int32 years) {
	SubtractMonth(years * 12);
	return *this;
}

DateTime DateTime::SubtractMonth(Int32 months) {
	DateAndTime current = ToDateAndTime();
	current.SubtractMonth(months);
	Tick = current.ToTick();
	return *this;
}

DateTime DateTime::SubtractDay(Int32 day) {
	const Int64U uiTick = Tick - day * TicksPerDay_v;
	CheckOverFlow(uiTick);
	Tick = uiTick;
	return *this;
}

DateTime DateTime::SubtractHour(const Int64 hour) {
	const Int64U uiTick = Tick - hour * TicksPerHour_v;
	CheckOverFlow(uiTick);
	Tick = uiTick;
	return *this;
}

DateTime DateTime::SubtractMinute(const Int64 minute) {
	const Int64U uiTick = Tick - minute * TicksPerMinute_v;
	CheckOverFlow(uiTick);
	Tick = uiTick;
	return *this;
}

DateTime DateTime::SubtractSecond(const Int64 second) {
	const Int64U uiTick = Tick - second * TicksPerSecond_v;
	CheckOverFlow(uiTick);
	Tick = uiTick;
	return *this;
}

DateTime DateTime::SubtractMiliSecond(const Int64 miliSecond) {
	const Int64U uiTick = Tick - miliSecond * TicksPerMiliSecond_v;
	CheckOverFlow(uiTick);
	Tick = uiTick;
	return *this;
}

DateTime DateTime::SubtractMicroSecond(const Int64 microSecond) {
	const Int64U uiTick = Tick - microSecond;
	CheckOverFlow(uiTick);
	Tick = uiTick;
	return *this;
}

DateTime DateTime::SubtractDateTime(const DateTime& other) {
	SubtractMicroSecond(other.Tick);
	return *this;
}

TimeSpan DateTime::Diff(const DateTime& other) const {
	return TimeSpan(Tick - other.Tick);
}

DateTime DateTime::operator-(const DateTime& other) const {
	DateTime temp(Tick);
	temp.SubtractMicroSecond(other.Tick);
	return temp;
}

DateTime DateTime::operator+(const DateTime& other) const {
	DateTime temp(Tick);
	temp.AddMicroSecond(other.Tick);
	return temp;
}

DateTime& DateTime::operator-=(const DateTime& other) {
	SubtractMicroSecond(other.Tick);
	return *this;
}

DateTime& DateTime::operator+=(const DateTime& other) {
	AddMicroSecond(other.Tick);
	return *this;
}

DateTime DateTime::operator-(const TimeSpan& other) const {
	DateTime temp(Tick);
	temp.SubtractMicroSecond(other.Tick);
	return temp;
}

DateTime DateTime::operator+(const TimeSpan& other) const {
	DateTime temp(Tick);
	temp.AddMicroSecond(other.Tick);
	return temp;
}

DateTime& DateTime::operator-=(const TimeSpan& other) {
	SubtractMicroSecond(other.Tick);
	return *this;
}

DateTime& DateTime::operator+=(const DateAndTime& other) {
	AddMicroSecond(other.ToTick());
	return *this;
}

DateTime DateTime::operator-(const DateAndTime& other) const {
	DateTime temp(Tick);
	temp.SubtractMicroSecond(other.ToTick());
	return temp;
}

DateTime DateTime::operator+(const DateAndTime& other) const {
	DateTime temp(Tick);
	temp.AddMicroSecond(other.ToTick());
	return temp;
}

DateTime& DateTime::operator-=(const DateAndTime& other) {
	SubtractMicroSecond(other.ToTick());
	return *this;
}

bool DateTime::operator>(const DateTime& other) const {
	return Compare(other) > 0;
}

bool DateTime::operator<(const DateTime& other) const {
	return Compare(other) < 0;
}

bool DateTime::operator>=(const DateTime& other) const {
	return Compare(other) >= 0;
}

bool DateTime::operator<=(const DateTime& other) const {
	return Compare(other) <= 0;
}

bool DateTime::operator==(const DateTime& other) const {
	return Compare(other) == 0;
}

bool DateTime::operator>(const DateAndTime& other) const {
	return Compare(other.ToTick()) > 0;
}

bool DateTime::operator<(const DateAndTime& other) const {
	return Compare(other.ToTick()) < 0;
}

bool DateTime::operator>=(const DateAndTime& other) const {
	return Compare(other.ToTick()) >= 0;
}

bool DateTime::operator<=(const DateAndTime& other) const {
	return Compare(other.ToTick()) <= 0;
}

bool DateTime::operator==(const DateAndTime& other) const {
	return Compare(other.ToTick()) == 0;
}

bool DateTime::operator>(const TimeSpan& other) const {
	return Compare(other.Tick) > 0;
}

bool DateTime::operator<(const TimeSpan& other) const {
	return Compare(other.Tick) < 0;
}

bool DateTime::operator>=(const TimeSpan& other) const {
	return Compare(other.Tick) >= 0;
}

bool DateTime::operator<=(const TimeSpan& other) const {
	return Compare(other.Tick) <= 0;
}

bool DateTime::operator==(const TimeSpan& other) const {
	return Compare(other.Tick) == 0;
}

/*=====================================================================================
	private non-static
	=====================================================================================*/
void DateTime::ReflectFormat(const DateAndTime& time, String& ret, const char token, const int count) const {
	if (token == '\0' || count == 0) {
		return;
	}

	const auto valuePtr = FormatTokenMap_v.Find(token);
	if (valuePtr == nullptr) {
		throw InvalidArgumentException("올바르지 않은 포맷 토큰입니다.");
	}

	auto [item1, item2, item3] = *valuePtr;

	const DateFormat format = static_cast<DateFormat>(static_cast<int>(item2) + count - 1);
	const int iMaxCount = item3; // 연속으로 나열가능한 문자 최대 수

	if (count > iMaxCount) {
		throw OutOfRangeException("토큰 갯수가 이상합니다. 확인하고 다시 입해주세요.");
	}

	switch (format) {
	case DateFormat::d:
		ret += StringUtil::Format("%d", time.Day);
		break;
	case DateFormat::dd:
		ret += StringUtil::Format("%02d", time.Day);
		break;
	case DateFormat::ddd:
		ret += GetAbbreviationWeekendName(GetDayOfWeek());
		break;
	case DateFormat::dddd:
		ret += GetFullWeekendName(GetDayOfWeek());
		break;
	case DateFormat::h:
		ret += StringUtil::Format("%d", time.Hour < 13 ? time.Hour : time.Hour - 12);
		break;
	case DateFormat::hh:
		ret += StringUtil::Format("%02d", time.Hour < 13 ? time.Hour : time.Hour - 12);
		break;
	case DateFormat::H:
		ret += StringUtil::Format("%d", time.Hour);
		break;
	case DateFormat::HH:
		ret += StringUtil::Format("%02d", time.Hour);
		break;
	case DateFormat::m:
		ret += StringUtil::Format("%d", time.Minute);
		break;
	case DateFormat::mm:
		ret += StringUtil::Format("%02d", time.Minute);
		break;
	case DateFormat::M:
		ret += StringUtil::Format("%d", time.Month);
		break;
	case DateFormat::MM:
		ret += StringUtil::Format("%02d", time.Month);
		break;
	case DateFormat::MMM:
		ret += GetAbbreviationMonthName(static_cast<MonthOfYear>(time.Month - 1));
		break;
	case DateFormat::MMMM:
		ret += GetFullMonthName(static_cast<MonthOfYear>(time.Month - 1));
		break;
	case DateFormat::s:
		ret += StringUtil::Format("%d", time.Second);
		break;
	case DateFormat::ss:
		ret += StringUtil::Format("%02d", time.Second);
		break;
	case DateFormat::t:
		ret += StringUtil::Format("%s", time.Hour / 12 > 0
			                                ? GetAbbreviationAMPMName(AMPM::PM)
			                                : GetAbbreviationAMPMName(AMPM::AM));
		break;
	case DateFormat::tt:
		ret += StringUtil::Format("%s", time.Hour / 12 > 0 ? GetFullAMPMName(AMPM::PM) : GetFullAMPMName(AMPM::AM));
		break;
	case DateFormat::y:
		ret += StringUtil::Format("%d", time.Year % 100);
		break;
	case DateFormat::yy:
		ret += StringUtil::Format("%02d", time.Year % 100);
		break;
	case DateFormat::yyy:
		ret += StringUtil::Format("%d", time.Year % 10000);
		break;
	case DateFormat::yyyy:
		ret += StringUtil::Format("%04d", time.Year % 10000);
		break;
	case DateFormat::K:
	case DateFormat::zzz: {
		const Int32 timezonBias = TimeZoneBiasMinute();
		ret += StringUtil::Format("%s%02d:%02d",
		                          timezonBias < 0 ? "+" : "", // %s
		                          (timezonBias * -1) / 60, // %02d
		                          (timezonBias * -1) % 60);
		break;
	}
	case DateFormat::z: {
		const Int32 timezonBias = TimeZoneBiasMinute();
		ret += StringUtil::Format("%s%d",
		                          timezonBias < 0 ? "+" : "", // %s
		                          (timezonBias * -1) / 60); // %d
		break;
	}
	case DateFormat::zz: {
		const Int32 timezonBias = TimeZoneBiasMinute();
		ret += StringUtil::Format("%s%02d",
		                          timezonBias < 0 ? "+" : "", // %s
		                          (timezonBias * -1) / 60); // %d
		break;
	}
	case DateFormat::f:
		ret += StringUtil::Format("%d", time.MiliSecond / 100);
		break;
	case DateFormat::ff:
		ret += StringUtil::Format("%02d", time.MiliSecond / 10);
		break;
	case DateFormat::fff:
		ret += StringUtil::Format("%03d", time.MiliSecond / 1);
		break;
	case DateFormat::ffff:
	case DateFormat::fffff:
	case DateFormat::ffffff:
		const int miliMicro = time.MiliSecond * MaxMiliSecond_v + time.MicroSecond;
		if (count == 4)
			ret += StringUtil::Format("%04d", miliMicro / 100);
		else if (count == 5)
			ret += StringUtil::Format("%05d", miliMicro / 10);
		else if (count == 6)
			ret += StringUtil::Format("%06d", miliMicro / 1);
		break;
	}
}



/*=====================================================================================
	private static
=====================================================================================*/

void DateTime::CheckOverFlow(Int64U tick) {
	if (tick >= Ticks10000Years_v) {
		throw OverFlowException("1만년을 넘길 수 없습니다.");
	}
}

Tuple<int, int, int, int, int> DateTime::GetYearsFromDays(int days) {
	using namespace Detail;

	const int i400Years = days / DaysPer400Years_v; // 총 일수에서 400년의 수를 구한다.
	days -= DaysPer400Years_v * i400Years; // 400년의 수를 빼준다.

	const int i100Years = days / DaysPer100Years_v; // 총 일수에서 100년의 수를 구한다.
	days -= DaysPer100Years_v * i100Years; // 100년의 수를 빼준다.

	const int i4Years = days / DaysPer4Years_v; // 총 일수에서 4년의 수를 구한다.
	days -= DaysPer4Years_v * i4Years; // 4년의 수를 빼준다.

	const int i1Years = days / DaysPer1Years_v; // 남은 일수에서 1년의 수를 구한다.
	days -= DaysPer1Years_v * i1Years; // 년단위는 모두 소거되고 366일 이내의 값이 남음

	return {i400Years, i100Years, i4Years, i1Years, days};
}

int DateTime::GetDatePart(const Int64 tick, const DatePart part) {
	const DateTime dateTime(tick);

	auto [i400Years, i100Years, i4Years, i1Years, iLeftDays] = GetYearsFromDays(static_cast<int>(dateTime.GetTotalDays()));

	// 1년이 365.2545일이므로 366일 째에 1일이 남는 경우 때문에
	if (i1Years == 4)
		i1Years = 3;

	// 4년, 즉 마지막년은 366일이므로
	const int* pUntilDays = i1Years == 3 ? (int*)DaysUntilMonth366_v : (int*)DaysUntilMonth365_v;
	const int* pForDays = i1Years == 3 ? (int*)DaysForMonth366_v : (int*)DaysForMonth365_v;


	if (part == DatePart::Year) {
		return i400Years * 400 + i100Years * 100 + i4Years * 4 + i1Years + 1; // 년도도 1년부터 시작이기 때문에 + 1
	}
	if (part == DatePart::DayOfYear) {
		return iLeftDays + 1;
	}

	int iMonth = 0;

	for (int i = 11; i >= 0; i--) {
		if (iLeftDays >= pUntilDays[i]) {
			iMonth = i;
			break;
		}
	}

	if (part == DatePart::Month) {
		return iMonth + 1; // 1월부터 시작이므로
	}
	if (part == DatePart::Day) {
		return iLeftDays - pUntilDays[iMonth] + 1; // 1일부터 시작이므로
	}
	if (part == DatePart::MaxDayOfMonth) {
		return pForDays[iMonth];
	}

	return -1;
}


/*=====================================================================================
									TimeSpan
=====================================================================================*/
TimeSpan::TimeSpan(Int32 days, Int64 hours, Int64 minutes, Int64 seconds, Int64 miliSeconds, Int64 microSeconds) {
	Tick = 0;
	Tick += days * TicksPerDay_v;
	Tick += hours * TicksPerHour_v;
	Tick += minutes * TicksPerMinute_v;
	Tick += seconds * TicksPerSecond_v;
	Tick += miliSeconds * TicksPerMiliSecond_v;
	Tick += microSeconds * TicksPerMicroSecond_v;
}


/*=====================================================================================
									Clock
=====================================================================================*/

Int64U StopWatch<StopWatchMode::System>::Start() {
	return StartTick = ::GetTickCount();
}

TimeSpan StopWatch<StopWatchMode::System>::Stop() {
	Int32U uiStopTick = ::GetTickCount();
	Int32U uiGap = uiStopTick - StartTick;
	StartTick = uiStopTick;
	return uiGap * 1'000;
}

TimeSpan StopWatch<StopWatchMode::System>::GetElapsed() {
	return (::GetTickCount() - StartTick) * 1'000;
}


// @함수 설명 참고: https://learn.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter
StopWatch<StopWatchMode::HighResolution>::StopWatch() {
	if (!QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency)) {
		DebugAssertMsg(false, "쿼리퍼포먼스 프리퀀시 획득 실패 (오류코드: %d)", ::GetLastError());
	}

	// 기본: Freqency.QuadPart: 10'000'000
	// 백의 자리 나노단위까지 정밀하게 측정가능
	// TimeSpan은 마이크로초 단위까지만 이쁘게 표현가능하므로 마이크로초 단위로 변환해주자.
	DebugAssertMsg((Frequency / 1'000'000) > 0, "프리퀀시가 마이크로초 단위 정밀도를 커버하지 못합니다.");
	Precision = Frequency / 1'000'000;
}

Int64U StopWatch<StopWatchMode::HighResolution>::Start() {
	if (!QueryPerformanceCounter((LARGE_INTEGER*)&StartCounter)) {
		DebugAssertMsg(false, "쿼리퍼포먼스 카운터 획득 실패 (오류코드: %d)", ::GetLastError());
	}

	return StartCounter;
}

TimeSpan StopWatch<StopWatchMode::HighResolution>::Stop() {
	Int64U StopCounter;
	if (!QueryPerformanceCounter((LARGE_INTEGER*)&StopCounter)) {
		DebugAssertMsg(false, "쿼리퍼포먼스 카운터 획득 실패 (오류코드: %d)", ::GetLastError());
	}

	Int64U uiGap = StopCounter - StartCounter;
	StartCounter = StopCounter;
	return uiGap / Precision;
}

TimeSpan StopWatch<StopWatchMode::HighResolution>::GetElapsed() {
	Int64U StopCounter;
	if (!QueryPerformanceCounter((LARGE_INTEGER*)&StopCounter)) {
		DebugAssertMsg(false, "쿼리퍼포먼스 카운터 획득 실패 (오류코드: %d)", ::GetLastError());
	}

	return (StopCounter - StartCounter) / Precision;
}

NS_JC_END
