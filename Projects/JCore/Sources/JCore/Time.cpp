/*
	�ۼ��� : ������
	������ �ð��� �ٷ� �� �ִ� ����� �߰��մϴ�.
	�����ġ ���
*/

#include <JCore/Core.h>
#include <JCore/Time.h>
#include <JCore/Math.h>
#include <JCore/String.h>
#include <JCore/Hasher.h>
#include <JCore/Ascii.h>
#include <JCore/Exception.h>
#include <JCore/Container/HashMap.h>

#include <chrono>
#include <timezoneapi.h>

using namespace std;

namespace JCore {

using namespace Detail;

/*=====================================================================================
									Date
=====================================================================================*/

Date::Date(Int32 year, Int32 month, Int32 day) {
	if (year < 1 || year > 9999) 
		throw InvalidArgumentException("���� 1 ~ 9999����� �����մϴ�.");
	if (month < 1 || month > 12)
		throw InvalidArgumentException("���� 1 ~ 12���� �����մϴ�.");
	if (day < 1 || day > 31)
		throw InvalidArgumentException("���� 1 ~ 31���� �����մϴ�.");

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

	Int32 iTempMonth = Int32(Month);
	Int32 iTempYear = Int32(Year);

	iTempMonth += months;

	if (iTempMonth > 12) {
		int iAddedYear = (iTempMonth - 1) / 12;
		iTempYear += iAddedYear;
		iTempMonth -= iAddedYear * 12;
	}

	if (iTempYear >= 10000) {
		throw OverFlowException("Date �� ���� ������ ������ �߻��Ͽ����ϴ�. 1������ �ѱ� �� �����ϴ�.");
	}

	Month = Int32(iTempMonth);
	Year = Int32(iTempYear);

	// ���� 1�� 31�ϳ� 13������ ���ߴٰ� ���� �� 
	// �� ������ �����ع����� 2�� 31���� �Ǿ������. (2���� ���������� ������ ��� 29�� ����� ��� 28�� ��)
	// �׷��� ����/��⿡ ���� �ش� ���� �ִ��� ���� �ʰ��ϴ� ��� �ִ� �ϼ��� �����������
	if (DateTime::IsLeapYear(Year)) {
		Day = Day > DaysForMonth366[Month - 1] ? DaysForMonth366[Month - 1] : Day;
	} else {
		Day = Day > DaysForMonth365[Month - 1] ? DaysForMonth365[Month - 1] : Day;
	}
}

void Date::AddDay(Int32 day) {
	Int64 tick = (Int64)ToTick() + day * TicksPerDay;

	if (tick < 0) {
		throw UnderFlowException("Date �� ���� ������ ������ �߻��Ͽ����ϴ�. �� ���� �������� ������ ������ �ȵ˴ϴ�.");
	}

	Date date = DateTime(tick).ToDate();

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

	Int32 iSubtractedYear = months / 12;
	Int32 iTempYear = Int32(Year);
	Int32 iTempMonth = Int32(Month);
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
		throw UnderFlowException("Date �� ���� ������ ������ �߻��Ͽ����ϴ�. �⵵�� ���� 0���ϰ� �� �� �����ϴ�.");
	}

	Year = Int16(iTempYear);
	Month = Int8(iTempMonth);

	if (DateTime::IsLeapYear(Year)) {
		Day = Day > DaysForMonth366[Month - 1] ? DaysForMonth366[Month - 1] : Day;
	} else {
		Day = Day > DaysForMonth365[Month - 1] ? DaysForMonth365[Month - 1] : Day;
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


Date Date::operator-(const Date& other) {
	Date temp(*this);
	temp.SubtractDate(other);
	return temp;
}

Date Date::operator+(const Date& other) {
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

	int i400Years = iCurrentYear / 400;
	iCurrentYear -= i400Years * 400;

	int i100Years = iCurrentYear / 100;
	iCurrentYear -= i100Years * 100;

	int i4Years = iCurrentYear / 4;
	iCurrentYear -= i4Years * 4;

	int i1Years = iCurrentYear;
			
	iTotalDays = i400Years * Detail::DaysPer400Years_v;
	iTotalDays += i100Years * Detail::DaysPer100Years_v;
	iTotalDays += i4Years * Detail::DaysPer4Years_v;
	iTotalDays += i1Years * Detail::DaysPer1Years_v;

	// ���� ���� ��, ���� 0��, 0������ ���۾ƴ϶� 1��, 1�� ���� �����̱⶧���� 1�� ������Ѵ�.
	if (DateTime::IsLeapYear(Year)) {
		iTotalDays += Detail::DaysUntilMonth366[Month - 1] + Day - 1;	
	} else {
		iTotalDays += Detail::DaysUntilMonth365[Month - 1] + Day - 1;
	}

	return iTotalDays * Detail::TicksPerDay;
}


/*=====================================================================================
										Time
	=====================================================================================*/

Time::Time(Int32 hour, Int32 minute, Int32 second, Int32 miliSecond, Int32 microSecond) {

	if (hour < 0 || hour > 23)
		throw InvalidArgumentException("�ð��� 0 ~ 23������ �����մϴ�.");
	if (minute < 0 || minute > 59)
		throw InvalidArgumentException("���� 0 ~ 59������ �����մϴ�.");
	if (second < 0 || second > 59)
		throw InvalidArgumentException("�ʴ� 0 ~ 59������ �����մϴ�.");
	if (miliSecond < 0 || miliSecond > 999)
		throw InvalidArgumentException("�и��ʴ� 0 ~ 999������ �����մϴ�.");
	if (microSecond < 0 || microSecond > 999)
		throw InvalidArgumentException("����ũ���ʴ� 0 ~ 999������ �����մϴ�.");

	Hour = hour; 
	Minute = minute;
	Second = second;
	MiliSecond = miliSecond;
	MicroSecond = microSecond;
}

void Time::AddHour(Int64 hours) {
	AddMicroSecond(hours * TicksPerHour, TimeUnit::Hour);
}

void Time::AddMinute(Int64 minutes) {
	AddMicroSecond(minutes * TicksPerMinute, TimeUnit::Minute);
}
		
void Time::AddSecond(Int64 seconds) {
	AddMicroSecond(seconds * TicksPerSecond, TimeUnit::Second);
}

void Time::AddMiliSecond(Int64 miliSeconds) {
	AddMicroSecond(miliSeconds * TicksPerMiliSecond, TimeUnit::MiliSecond);
}

void Time::AddMicroSecond(Int64 microSeconds, TimeUnit timeUnit) {
	Int64 iTick = Int64(ToTick()) + microSeconds;

	if (iTick < 0) {
		throw UnderFlowException("Time ���� ������ ������ �߻��Ͽ����ϴ�. ������ ������ �ȵ˴ϴ�.");
	}

	Int64 uiHour = iTick / TicksPerHour;
	Hour = uiHour % MaxHour_v;

	if (timeUnit == TimeUnit::Hour)
		return;

	Int64 uiMinute = iTick / TicksPerMinute;
	Minute = uiMinute % MaxMinute_v;

	if (timeUnit == TimeUnit::Minute)
		return;

	Int64 uiSecond = iTick / TicksPerSecond;
	Second = uiSecond % MaxSecond_v;

	if (timeUnit == TimeUnit::Second)
		return;

	Int64 uiMiliSecond = iTick / TicksPerMiliSecond;
	MiliSecond = uiMiliSecond % MaxMiliSecond_v;

	if (timeUnit == TimeUnit::MiliSecond)
		return;


	MicroSecond = iTick % MaxMicroSecond_v;
}

void Time::AddTime(const Time& other) {
	Int64 iTick = ToTick() + other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("Time ���� ������ ������ �߻��Ͽ����ϴ�. ������ ������ �ȵ˴ϴ�.");
	}

	*this = DateTime(iTick).ToTime();
}

void Time::SubtractHour(Int64 hours) {
	SubtractMicroSecond(hours * TicksPerHour, TimeUnit::Hour);
}

void Time::SubtractMinute(Int64 minutes) {
	SubtractMicroSecond(minutes * TicksPerMinute, TimeUnit::Minute);
}

void Time::SubtractSecond(Int64 seconds) {
	SubtractMicroSecond(seconds * TicksPerSecond, TimeUnit::Second);
}

void Time::SubtractMiliSecond(Int64 miliSeconds) {
	SubtractMicroSecond(miliSeconds * TicksPerMiliSecond, TimeUnit::MiliSecond);
}

void Time::SubtractMicroSecond(Int64 microSeconds, TimeUnit timeUnit) {
	if (microSeconds < 0) {
		AddMicroSecond(Math::Abs(microSeconds));
		return;
	}

	AddMicroSecond(microSeconds * -1);
}

void Time::SubtractTime(const Time& other) {
	Int64 iTick = ToTick() - other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("Time ���� ������ ������ �߻��Ͽ����ϴ�. ������ ������ �ȵ˴ϴ�.");
	}

	*this = DateTime(iTick).ToTime();
}


Time Time::operator-(const Time& other) {
	Time temp(*this);
	temp.SubtractTime(other);
	return temp;
}

Time Time::operator+(const Time& other) {
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

bool Time::operator>(const Time& other) {
	return Compare(other) > 0;
}

bool Time::operator<(const Time& other) {
	return Compare(other) < 0;
}

bool Time::operator>=(const Time& other) {
	return Compare(other) >= 0;
}

bool Time::operator<=(const Time& other) {
	return Compare(other) <= 0;
}

bool Time::operator==(const Time& other) {
	return Compare(other) == 0;
}

Int64 Time::ToTick() const {
	Int64 uiTotalTick = 0;
	uiTotalTick += Hour * TicksPerHour;
	uiTotalTick += Minute * TicksPerMinute;
	uiTotalTick += Second * TicksPerSecond;
	uiTotalTick += MiliSecond * TicksPerMiliSecond;
	uiTotalTick += MicroSecond * TicksPerMicroSecond;
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

void DateAndTime::AddDay(Int32 days) {
	AddMicroSecond(days * TicksPerDay);
}

void DateAndTime::AddHour(Int64 hours) {
	AddMicroSecond(hours * TicksPerHour);
}

void DateAndTime::AddMinute(Int64 minutes) {
	AddMicroSecond(minutes * TicksPerMinute);
}

void DateAndTime::AddSecond(Int64 seconds) {
	AddMicroSecond(seconds * TicksPerSecond);
}

void DateAndTime::AddMiliSecond(Int64 miliSeconds) {
	AddMicroSecond(miliSeconds * TicksPerMiliSecond);
}
		
void DateAndTime::AddMicroSecond(Int64 microSeconds, TimeUnit timeUnit) {
	Int64 iTick = ToTick() + microSeconds;

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime ���� ������ ������ �߻��Ͽ����ϴ�. ������ ������ �ȵ˴ϴ�.");
	}

	if (iTick >= Ticks10000Years) {
		throw UnderFlowException("DateAndTime ���� ������ ������ �߻��Ͽ����ϴ�. 1���� �̻��� �� �� �����ϴ�.");
	}
	DateTime current(iTick);
	*this = current.ToDateAndTime();
}

void DateAndTime::AddDate(const Date& other) {
	Int64 iTick = ToTick() + other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime ���� ������ ������ �߻��Ͽ����ϴ�. ������ ������ �ȵ˴ϴ�.");
	}

	if (iTick >= Ticks10000Years) {
		throw UnderFlowException("DateAndTime ���� ������ ������ �߻��Ͽ����ϴ�. 1���� �̻��� �� �� �����ϴ�.");
	}
	DateTime current(iTick);
	*this = current.ToDateAndTime();
}

void DateAndTime::AddTime(const Time& other) {
	Int64 iTick = ToTick() + other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime ���� ������ ������ �߻��Ͽ����ϴ�. ������ ������ �ȵ˴ϴ�.");
	}

	DateTime current(iTick);
	*this = current.ToDateAndTime();
}

void DateAndTime::AddDateAndTime(const DateAndTime& other) {
	Int64 iTick = ToTick() + other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime ���� ������ ������ �߻��Ͽ����ϴ�. ������ ������ �ȵ˴ϴ�.");
	}

	if (iTick >= Ticks10000Years) {
		throw UnderFlowException("DateAndTime ���� ������ ������ �߻��Ͽ����ϴ�. 1���� �̻��� �� �� �����ϴ�.");
	}

	DateTime current(iTick);
	*this = current.ToDateAndTime();
}

void DateAndTime::SubtractYear(Int32 years) {
	Date::SubtractMonth(years * 12);
}
void DateAndTime::SubtractMonth(Int32 months) {
	Date::SubtractMonth(months);
}
void DateAndTime::SubtractDay(Int32 days) {
	SubtractMicroSecond(days * TicksPerDay);
}

void DateAndTime::SubtractHour(Int64 hours) {
	SubtractMicroSecond(hours * TicksPerHour);
}

void DateAndTime::SubtractMinute(Int64 minutes) {
	SubtractMicroSecond(minutes * TicksPerMinute);
}

void DateAndTime::SubtractSecond(Int64 seconds) {
	SubtractMicroSecond(seconds * TicksPerSecond);
}

void DateAndTime::SubtractMiliSecond(Int64 miliSeconds) {
	SubtractMicroSecond(miliSeconds * TicksPerMiliSecond);
}

void DateAndTime::SubtractMicroSecond(Int64 microSeconds, TimeUnit timeUnit) {
	Int64 iTick = ToTick() - microSeconds;

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime ���� ������ ������ �߻��Ͽ����ϴ�. ������ ������ �ȵ˴ϴ�.");
	}
	DateTime current(iTick);
	*this = current.ToDateAndTime();
}

void DateAndTime::SubtractDate(const Date& other) {
	Int64 iTick = ToTick() - other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime ���� ������ ������ �߻��Ͽ����ϴ�. ������ ������ �ȵ˴ϴ�.");
	}

	DateTime current(iTick);
	*this = current.ToDateAndTime();
}

void DateAndTime::SubtractTime(const Time& other) {
	Int64 iTick = ToTick() - other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime ���� ������ ������ �߻��Ͽ����ϴ�. ������ ������ �ȵ˴ϴ�.");
	}

	DateTime current(iTick);
	*this = current.ToDateAndTime();
}

void DateAndTime::SubtractDateAndTime(const DateAndTime& other) {
	SubtractMicroSecond(other.ToTick());
}

DateAndTime DateAndTime::operator-(const DateAndTime& other) {
	Int64 iTick = ToTick() - other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime ���� ������ ������ �߻��Ͽ����ϴ�. ������ ������ �ȵ˴ϴ�.");
	}

	return DateTime(iTick).ToDateAndTime();
}

DateAndTime DateAndTime::operator+(const DateAndTime& other) {
	Int64 iTick = ToTick() + other.ToTick();

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime ���� ������ ������ �߻��Ͽ����ϴ�. ������ ������ �ȵ˴ϴ�.");
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

DateAndTime DateAndTime::operator-(const DateTime& other) {
	Int64 iTick = ToTick() - other.m_Tick;

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime ���� ������ ������ �߻��Ͽ����ϴ�. ������ ������ �ȵ˴ϴ�.");
	}

	return DateTime(iTick).ToDateAndTime();
}

DateAndTime DateAndTime::operator+(const DateTime& other) {
	Int64 iTick = ToTick() + other.m_Tick;

	if (iTick < 0) {
		throw UnderFlowException("DateAndTime ���� ������ ������ �߻��Ͽ����ϴ�. ������ ������ �ȵ˴ϴ�.");
	}

	return DateTime(iTick).ToDateAndTime();
}

DateAndTime& DateAndTime::operator-=(const DateTime& other) {
	SubtractMicroSecond(other.m_Tick);
	return *this;
}

DateAndTime& DateAndTime::operator+=(const DateTime& other) {
	AddMicroSecond(other.m_Tick);
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

bool DateAndTime::operator>(const DateTime& other) {
	return ToTick() > other.m_Tick;
}

bool DateAndTime::operator<(const DateTime& other) {
	return ToTick() < other.m_Tick;
}

bool DateAndTime::operator>=(const DateTime& other) {
	return ToTick() >= other.m_Tick;
}

bool DateAndTime::operator<=(const DateTime& other) {
	return ToTick() <= other.m_Tick;
}

bool DateAndTime::operator==(const DateTime& other) {
	return ToTick() == other.m_Tick;
}

/*=====================================================================================
									DateTime
	=====================================================================================*/

const char* DateTime::ms_szWeekAbbrevName[] = { "Mon", "Tue" , "Wed" , "Thu" , "Fri" , "Sat", "Sun" };
const char* DateTime::ms_szWeekFullName[] = { "Monday", "Tuesday" , "Wednesday" , "Thursday" , "Friday" , "Saturday", "Sunday" };
const char* DateTime::ms_szMonthAbbrevName[] = { "Jan", "Feb" , "Mar" , "Apr" , "May" , "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
const char* DateTime::ms_szMonthFullName[] = { "January", "February" , "March" , "April" , "May" , "June", "July", "August", "September", "October", "November", "December" };
const char* DateTime::ms_szAMPMAbbrevName[] = { "A", "P" };
const char* DateTime::ms_szAMPMFullName[] = { "AM", "PM" };

/*
	Ű : char (���� ���� �չ���)
	�� : Tuple<char, Format, int>

			char : ���� ���� �չ���
			Format : ���� �̳� ���� ����
			int : ������ ������ �ִ� ���� ����

	������ ���� �м��� y�� ���� ���� ���� ��� Format::y + 2�� �ش� ���� ������ ã�ư����� �Ѵ�.
	�׸��� y�� ���� 5�� �������� ���. �� �ش� ���� ���ڿ��� ������ �ִ� ���� ������ �ʰ��� ���� ������ �ٿ�� ����
*/

HashMap<char, Tuple<char, DateFormat, int>> FormatTokenMap_v =
{
	{ 'd', {'d', DateFormat::d, 4 } },
	{ 'h', {'h', DateFormat::h, 2 } },
	{ 'H', {'H', DateFormat::H, 2 } },
	{ 'm', {'m', DateFormat::m, 2 } },
	{ 'M', {'M', DateFormat::M, 4 } },
	{ 's', {'s', DateFormat::s, 2 } },
	{ 't', {'t', DateFormat::t, 2 } },
	{ 'y', {'y', DateFormat::y, 4 } },
	{ 'K', {'K', DateFormat::K, 1 } },
	{ 'z', {'z', DateFormat::z, 3 } },
	{ 'f', {'f', DateFormat::f, 6 } }
};


/*=====================================================================================
	public
	=====================================================================================*/
inline DateAndTime DateTime::ToDateAndTime() const {
		return DateAndTime(Int32U(GetYear()), Int32U(GetMonth()), Int32U(GetDay()),	// Date ����
							Int32U(GetHour()), Int32U(GetMinute()), Int32U(GetSecond()), Int32U(GetMiliSecond()), Int32U(GetMicroSecond())); // Time ����
}

inline Date DateTime::ToDate() const {
	return Date(Int32U(GetYear()), Int32U(GetMonth()), Int32U(GetDay()));
}

inline Time DateTime::ToTime() const {
	return Time(Int32U(GetHour()), Int32U(GetMinute()), Int32U(GetSecond()), Int32U(GetMiliSecond()), Int32U(GetMicroSecond()));
}



// ���˿� ���� �ð� ���ڿ��� �򵵷� �Ѵ�.
// @���� ���۷��� : https://www.c-sharpcorner.com/blogs/date-and-time-format-in-c-sharp-programming1
// C# ���� ���� ������ �����ϴ�.
		
// O(n)
String DateTime::Format(const char* fmt) const {
	String szFmt(fmt);
	String szRet(szFmt.Capacity() * 2);

	int iContinuousCount = 0;
	char cContinuousToken = '\0';

	const DateAndTime currentDateAndTime = ToDateAndTime();

	for (int i = 0; i < szFmt.Length(); i++) {
		if (FormatTokenMap_v.Exist(szFmt[i])) {
				
			if (cContinuousToken != szFmt[i]) {
				// ���� ��ū�ϰ� �ٸ� ��� = ó�� �߰��� ���
				ReflectFormat(currentDateAndTime, szRet, cContinuousToken, iContinuousCount);
				cContinuousToken = szFmt[i];
				iContinuousCount = 1;
			} else {
				// ���� ��ū�ϰ� ��ġ�ϰ� �ִ� ���
				iContinuousCount++;
			}
		} else {
			if (IsAlphabat(szFmt[i])) {
				throw InvalidArgumentException("��ū ���ڿ��� ���ĺ� ���ڰ� ���˹��ڿ��� ���ԵǾ� �ֽ��ϴ�.");
			}

			// ��ū�� �ƴ� �ٸ� ���ڸ� �߰��� ���
			ReflectFormat(currentDateAndTime, szRet, cContinuousToken, iContinuousCount);
			cContinuousToken = '\0';
			szRet += szFmt[i];
			iContinuousCount = 0;
		}
	}

	ReflectFormat(currentDateAndTime, szRet, cContinuousToken, iContinuousCount);

	return szRet;
}
		

// Ÿ���� �ð� ���� ��� �Լ�
// @���� : https://docs.microsoft.com/ko-kr/windows/win32/api/timezoneapi/nf-timezoneapi-gettimezoneinformation?redirectedfrom=MSDN (Ÿ���� ���� ��¹�)
Int32 DateTime::TimeZoneBiasMinute() {
	TIME_ZONE_INFORMATION timeZoneInformation;
	GetTimeZoneInformation(&timeZoneInformation);
	Int32 bias = timeZoneInformation.Bias;
	return bias;
}

// ����ð� ��� �Լ�
// @���� : https://stackoverflow.com/questions/15957805/extract-year-month-day-etc-from-stdchronotime-point-in-c (����ð� ��� ���)
// @���� : https://stackoverflow.com/questions/32811729/add-time-duration-to-c-timepoint (�ð� ������)
DateTime DateTime::Now(TimeStandard timeStandard) {
	chrono::system_clock::time_point now = chrono::system_clock::now();

	// Unix Epoch UTC �ð��� ����ũ���� ������ ���Ѵ�.
	//  = 1970�� 1�� 1�Ϻ��� ��������� ����ũ����
	Int64 epoch = chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()).count(); 

	// After Christ (0001�� 1�� 1�� ~ 1969�� 12�� 31��)������ ����ũ���ʸ� ���������ν�
	// 0001�� 1�� 1�� ~ ��������� After Chirst UTC �ð��� ���Ѵ�.
	epoch += ADBeginTick;

	// ���� �ð��� Ÿ���� ������ŭ �����ش�.
	if (timeStandard == TimeStandard::Local) {
		Int32 uiBias = TimeZoneBiasMinute();
		epoch += (uiBias * -1) * TicksPerMinute;
	}
	return DateTime(epoch);
}

// @���� ���� ���� : https://ko.wikipedia.org/wiki/%EC%9C%A4%EB%85%84 
bool DateTime::IsLeapYear(int year) {
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

DateTime DateTime::AddYear(Int32 years) {
	AddMonth(years * 12);
	return *this;
}

DateTime DateTime::AddMonth(Int32 months) {
	DateAndTime current = ToDateAndTime();
	current.AddMonth(months);
	m_Tick = current.ToTick();
	return *this;
}

DateTime DateTime::AddDay(Int32 day) {
	m_Tick += day * Detail::TicksPerDay;
	CheckUnderFlow();
	CheckOverFlow();
	return *this;
}
DateTime DateTime::AddHour(Int64 hour) {
	m_Tick += hour * Detail::TicksPerHour;
	CheckUnderFlow();
	CheckOverFlow();
	return *this;
}
DateTime DateTime::AddMinute(Int64 minute) {
	m_Tick += minute * Detail::TicksPerMinute;
	CheckUnderFlow();
	CheckOverFlow();
	return *this;
}
DateTime DateTime::AddSecond(Int64 second) {
	m_Tick += second * Detail::TicksPerSecond;
	CheckUnderFlow();
	CheckOverFlow();
	return *this;
}
DateTime DateTime::AddMiliSecond(Int64 miliSecond) {
	m_Tick += miliSecond * Detail::TicksPerMiliSecond;
	CheckUnderFlow();
	CheckOverFlow();
	return *this;
}

DateTime DateTime::AddMicroSecond(Int64 microSecond) {
	m_Tick += microSecond;
	CheckUnderFlow();
	CheckOverFlow();
	return *this;
}

DateTime DateTime::AddDateTime(const DateTime& other) {
	AddMicroSecond(other.m_Tick);
	return *this;
}

DateTime DateTime::SubtractYear(Int32 years) {
	SubtractMonth(years * 12);
	return *this;
}

DateTime DateTime::SubtractMonth(Int32 months) {
	DateAndTime current = ToDateAndTime();
	current.SubtractMonth(months);
	m_Tick = current.ToTick();
	return *this;
}

DateTime DateTime::SubtractDay(Int32 day) {
	m_Tick -= day * Detail::TicksPerDay;
	CheckUnderFlow();
	CheckOverFlow();
	return *this;
}

DateTime DateTime::SubtractHour(Int64 hour) {
	m_Tick -= hour * Detail::TicksPerHour;
	CheckUnderFlow();
	CheckOverFlow();
	return *this;
}

DateTime DateTime::SubtractMinute(Int64 minute) {
	m_Tick -= minute * Detail::TicksPerMinute;
	CheckUnderFlow();
	CheckOverFlow();
	return *this;
}

DateTime DateTime::SubtractSecond(Int64 second) {
	m_Tick -= second * Detail::TicksPerSecond;
	CheckUnderFlow();
	CheckOverFlow();
	return *this;
}

DateTime DateTime::SubtractMiliSecond(Int64 miliSecond) {
	m_Tick -= miliSecond * Detail::TicksPerMiliSecond;
	CheckUnderFlow();
	CheckOverFlow();
	return *this;
}

DateTime DateTime::SubtractMicroSecond(Int64 microSecond) {
	m_Tick -= microSecond;
	CheckUnderFlow();
	CheckOverFlow();
	return *this;
}

DateTime DateTime::SubtractDateTime(const DateTime& other) {
	SubtractMicroSecond(other.m_Tick);
	return *this;
}

TimeSpan DateTime::Diff(const DateTime& other) {
	return TimeSpan(m_Tick - other.m_Tick);
}

DateTime DateTime::operator-(const DateTime& other) const {
	DateTime temp(m_Tick);
	temp.SubtractMicroSecond(other.m_Tick);
	return temp;
}

DateTime DateTime::operator+(const DateTime& other) const {
	DateTime temp(m_Tick);
	temp.AddMicroSecond(other.m_Tick);
	return temp;
}

DateTime& DateTime::operator-=(const DateTime& other) {
	SubtractMicroSecond(other.m_Tick);
	return *this;
}

DateTime& DateTime::operator+=(const DateTime& other) {
	AddMicroSecond(other.m_Tick);
	return *this;
}

DateTime DateTime::operator-(const TimeSpan& other) const {
	DateTime temp(m_Tick);
	temp.SubtractMicroSecond(other.Tick);
	return temp;
}

DateTime DateTime::operator+(const TimeSpan& other) const {
	DateTime temp(m_Tick);
	temp.AddMicroSecond(other.Tick);
	return temp;
}

DateTime& DateTime::operator-=(const TimeSpan& other){
	SubtractMicroSecond(other.Tick);
	return *this;
}

DateTime& DateTime::operator+=(const DateAndTime& other) {
	AddMicroSecond(other.ToTick());
	return *this;
}

DateTime DateTime::operator-(const DateAndTime& other) const {
	DateTime temp(m_Tick);
	temp.SubtractMicroSecond(other.ToTick());
	return temp;
}

DateTime DateTime::operator+(const DateAndTime& other) const {
	DateTime temp(m_Tick);
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
void DateTime::ReflectFormat(const DateAndTime& time, String& ret, char token, int count) const {
	if (token == '\0' || count == 0) {
		return;
	}

	auto valuePtr = FormatTokenMap_v.Find(token);
	if (valuePtr == nullptr) {
		throw InvalidArgumentException("�ùٸ��� ���� ���� ��ū�Դϴ�.");
	}

	auto tokenTuple = *valuePtr;

	const DateFormat format = DateFormat(int(tokenTuple.item2) + count - 1);
	const int iMaxCount = tokenTuple.item3;	// �������� ���������� ���� �ִ� ��

	if (count > iMaxCount) {
		throw OutOfRangeException("��ū ������ �̻��մϴ�. Ȯ���ϰ� �ٽ� �����ּ���.");
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
		ret += GetAbbreviationMonthName(MonthOfYear(time.Month - 1));
		break;
	case DateFormat::MMMM:
		ret += GetFullMonthName(MonthOfYear(time.Month - 1));
		break;
	case DateFormat::s:
		ret += StringUtil::Format("%d", time.Second);
		break;
	case DateFormat::ss:
		ret += StringUtil::Format("%02d", time.Second);
		break;
	case DateFormat::t:
		ret += StringUtil::Format("%s", time.Hour / 12 > 0 ? GetAbbreviationAMPMName(AMPM::PM) : GetAbbreviationAMPMName(AMPM::AM));
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
		Int32 timezonBias = TimeZoneBiasMinute();
		ret += StringUtil::Format("%s%02d:%02d",
			timezonBias < 0 ? "+" : "",		// %s
			(timezonBias * -1) / 60,		// %02d
			(timezonBias * -1) % 60);
		break;
	}
	case DateFormat::z: {
		Int32 timezonBias = TimeZoneBiasMinute();
		ret += StringUtil::Format("%s%d",
			timezonBias < 0 ? "+" : "",		// %s
			(timezonBias * -1) / 60);		// %d
		break;
	}
	case DateFormat::zz: {
		Int32 timezonBias = TimeZoneBiasMinute();
		ret += StringUtil::Format("%s%02d",
			timezonBias < 0 ? "+" : "",		// %s
			(timezonBias * -1) / 60);		// %d
		break;
	}
	case DateFormat::f:
		ret += StringUtil::Format("%d", time.MiliSecond / 100);
		break;
	case DateFormat::ff:
		ret += StringUtil::Format("%d", time.MiliSecond / 10);
		break;
	case DateFormat::fff:
		ret += StringUtil::Format("%d", time.MiliSecond / 1);
		break;
	case DateFormat::ffff:
	case DateFormat::fffff:
	case DateFormat::ffffff:
		int miliMicro = time.MiliSecond * MaxMiliSecond_v + time.MicroSecond;
		if (count == 4)
			ret += StringUtil::Format("%d", miliMicro / 100);
		else if (count == 5)
			ret += StringUtil::Format("%d", miliMicro / 10);
		else if (count == 6)
			ret += StringUtil::Format("%d", miliMicro / 1);
		break;
	}
}

void DateTime::CheckUnderFlow() const {
	if (m_Tick < 0) {
		throw UnderFlowException("ƽ�� ������ �� �� �����ϴ�.");
	}
}

void DateTime::CheckOverFlow() const {
	if (m_Tick >= Ticks10000Years) {
		throw UnderFlowException("1������ �ѱ� �� �����ϴ�.");
	}
}

/*=====================================================================================
	private static
	=====================================================================================*/
Tuple<int, int, int, int, int> DateTime::GetYearsFromDays(int days) {
	using namespace Detail;

	int i400Years = days / DaysPer400Years_v;	// �� �ϼ����� 400���� ���� ���Ѵ�.
	days -= DaysPer400Years_v * i400Years;		// 400���� ���� ���ش�.

	int i100Years = days / DaysPer100Years_v;	// �� �ϼ����� 100���� ���� ���Ѵ�.
	days -= DaysPer100Years_v * i100Years;		// 100���� ���� ���ش�.

	int i4Years = days / DaysPer4Years_v;		// �� �ϼ����� 4���� ���� ���Ѵ�.
	days -= DaysPer4Years_v * i4Years;			// 4���� ���� ���ش�.

	int i1Years = days / DaysPer1Years_v;		// ���� �ϼ����� 1���� ���� ���Ѵ�.
	days -= DaysPer1Years_v * i1Years;			// ������� ��� �Ұŵǰ� 366�� �̳��� ���� ����

	return { i400Years, i100Years, i4Years, i1Years, days };
}

int DateTime::GetDatePart(const Int64 tick, const DatePart part) {
	DateTime dateTime(tick);

	auto years = GetYearsFromDays(dateTime.GetTotalDays());

	int i400Years = years.item1;
	int i100Years = years.item2;
	int i4Years = years.item3;
	int i1Years = years.item4;
	int iLeftDays = years.item5;

	// 1���� 365.2545���̹Ƿ� 366�� °�� 1���� ���� ��� ������
	if (i1Years == 4)
		i1Years = 3;

	// 4��, �� ���������� 366���̹Ƿ�
	int* pUntilDays = i1Years == 3 ? (int*)DaysUntilMonth366 : (int*)DaysUntilMonth365;
	int* pForDays = i1Years == 3 ? (int*)DaysForMonth366 : (int*)DaysForMonth365;


	if (part == DatePart::Year) {
		return i400Years * 400 + i100Years * 100 + i4Years * 4 + i1Years + 1;	// �⵵�� 1����� �����̱� ������ + 1
	} else if (part == DatePart::DayOfYear) {
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
		return iMonth + 1; // 1������ �����̹Ƿ�
	} else if (part == DatePart::Day) {
		return iLeftDays - pUntilDays[iMonth] + 1;	// 1�Ϻ��� �����̹Ƿ�
	} else if (part == DatePart::MaxDayOfMonth) {
		return pForDays[iMonth];
	}

	return -1;
}
		

/*=====================================================================================
									TimeSpan
=====================================================================================*/
TimeSpan::TimeSpan(Int32 days, Int64 hours, Int64 minutes, Int64 seconds, Int64 miliSeconds, Int64 microSeconds) {
	Tick = 0;
	Tick += days * TicksPerDay;
	Tick += hours * TicksPerHour;
	Tick += minutes * TicksPerMinute;
	Tick += seconds * TicksPerSecond;
	Tick += miliSeconds * TicksPerMiliSecond;
	Tick += microSeconds * TicksPerMicroSecond;
}

} // namespace JCore

