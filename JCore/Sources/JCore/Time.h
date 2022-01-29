/*
	�ۼ��� : ������
	������ �ð��� �ٷ� �� �ִ� ����� �߰��մϴ�.
	 - 100�� �ڸ� ���� ������ ���е��� �ð����� ��� 
	 - C# �ð�/��¥ ���� ���� ���̺귯��(System.DateTime) ����
	 - C++ std::chrono ���� ���̺귯�� ����
	 
	�����ġ ���
*/

#pragma once

#include <JCore/Type.h>


namespace JCore {

	namespace Detail {
		
		constexpr double MaxDay_v			= 365;
		constexpr Int64U MaxHour_v			= 24;
		constexpr Int64U MaxMinute_v		= 60;
		constexpr Int64U MaxSecond_v		= 60;
		constexpr Int64U MaxMiliSecond_v	= 1000;
		constexpr Int64U MaxMicroSecond_v	= 1000;
		constexpr Int64U MaxNanoSecond_v	= 1000;
		
		// 1ƽ�� 1������
		constexpr Int64U TicksPerMicroSecond	= MaxNanoSecond_v;							// ����ũ���ʴ� �� ƽ���� 	1000ƽ��						1����ũ����
		constexpr Int64U TicksPerMiliSecond		= TicksPerMicroSecond * MaxMicroSecond_v;	// �и��ʴ� �� ƽ����		1000000ƽ��					1�и���
		constexpr Int64U TicksPerSecond			= TicksPerMiliSecond * MaxMiliSecond_v;		// �ʴ� �� ƽ����			1000000000ƽ��				1��
		constexpr Int64U TicksPerMinute			= TicksPerSecond * MaxSecond_v;				// 1�д� �� ƽ����		1000000000 * 60				1��
		constexpr Int64U TicksPerHour			= TicksPerMinute * MaxMinute_v;				// 1�ð��� �� ƽ����		1000000000 * 60 * 60		1�ð�
		constexpr Int64U TicksPerDay			= TicksPerHour * MaxHour_v;					// 1�ϴ� �� ƽ����		1000000000 * 60 * 60 * 24	1��

		constexpr int DaysPer1Years_v = MaxDay_v;
		constexpr int DaysPer4Years_v = MaxDay_v * 4 + 1;				// 4���� �������� : 1461��
		constexpr int DaysPer100Years_v = DaysPer4Years_v * 25 - 1;		// 100���� �������� : 36524��
		constexpr int DaysPer400Years_v = DaysPer100Years_v * 4 + 1;	// 400���� �������� : 146097��
		
		constexpr int DaysUntilMonth365[13] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
		constexpr int DaysUntilMonth366[13] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

		constexpr int DaysForMonth365[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
		constexpr int DaysForMonth366[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

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

		enum class TimeStandard
		{
			Local,			// ���� ��ǻ�� �ý��� ���� �ð�
			Universal		// UTC ���ؽð�
		};
	}


struct Date
{
	Int16U Year;
	Int16U Month;
	Int16U Day;
};

struct Time
{
	Int16U Hour;
	Int16U Minute;
	Int16U Second;
	Int16U MiliSecond;
	Int16U MicroSecond;
	Int16U NanoSecond;
};

struct DateAndTime : Date, Time {};

class String;
class DateTime
{
public:
	constexpr DateTime() : m_Tick(0) {}
	constexpr DateTime(Int64U tick) : m_Tick(tick) {}
public:
	// Ư�� Ÿ�����ֿ� �ش��ϴ� ��ü�ð� ���
	inline constexpr Int64U GetTick() const { return m_Tick; }
	inline constexpr Int64U GetTotalDays() const { return m_Tick / Detail::TicksPerDay; }
	inline constexpr Int64U GetTotalHours() const { return m_Tick / Detail::TicksPerHour; }
	inline constexpr Int64U GetTotalMinutes() const { return m_Tick / Detail::TicksPerMinute; }
	inline constexpr Int64U GetTotalSeconds() const { return m_Tick / Detail::TicksPerSecond; }
	inline constexpr Int64U GetTotalMiliSeconds() const { return m_Tick / Detail::TicksPerMiliSecond; }
	inline constexpr Int64U GetTotalMicroSeconds() const { return m_Tick / Detail::TicksPerMicroSecond; }
	inline constexpr Int64U GetTotalNanoSeconds() const { return m_Tick; }

	// Ư�� Ÿ�����ֺ��� �ð� ���
	inline constexpr int GetYear() const { return GetDatePart(Detail::DatePart::Year); }
	inline constexpr int GetMonth() const { return GetDatePart(Detail::DatePart::Month); }
	inline constexpr int GetDay() const { return GetDatePart(Detail::DatePart::Day); }
	inline constexpr int GetHour() const { return GetTotalHours() % Detail::MaxHour_v; }
	inline constexpr int GetMinute() const { return GetTotalMinutes() % Detail::MaxMinute_v; }
	inline constexpr int GetSecond() const { return GetTotalSeconds() % Detail::MaxSecond_v; }
	inline constexpr int GetMiliSecond() const { return GetTotalMiliSeconds() % Detail::MaxMiliSecond_v; }
	inline constexpr int GetMicroSecond() const { return GetTotalMicroSeconds() % Detail::MaxMicroSecond_v; }
	inline constexpr int GetNanoSecond() const { return m_Tick % Detail::MaxNanoSecond_v; }

	// ���� ��������
	inline constexpr Detail::DayOfWeek GetDayOfWeek() const { return Detail::DayOfWeek(GetTotalDays() / 7 + 1); }

	// �ϳ�������� ��������
	inline constexpr int GetDayOfYear() const { return GetDatePart(Detail::DatePart::DayOfYear); }

	// �̹����� �ִ� ��������
	inline constexpr int GetMaxDayOfMonth() const { return GetDatePart(Detail::DatePart::MaxDayOfMonth); }

	// Ÿ�� ��ȯ
	inline constexpr DateAndTime ToDateAndTime() const {
		return { Int16U(GetYear()), Int16U(GetMonth()),	 Int16U(GetDay()),	// Date ����
				 Int16U(GetHour()), Int16U(GetMinute()), Int16U(GetSecond()), Int16U(GetMiliSecond()), Int16U(GetMicroSecond()), Int16U(GetNanoSecond()) };	// Time ����
	}
	inline constexpr Date ToDate() const {
		return { Int16U(GetYear()), Int16U(GetMonth()),	 Int16U(GetDay()) };
	}
	inline constexpr Time ToTime() const {
		return { Int16U(GetHour()), Int16U(GetMinute()), Int16U(GetSecond()), Int16U(GetMiliSecond()), Int16U(GetMicroSecond()), Int16U(GetNanoSecond()) };
	}
public:
	constexpr int GetDatePart(const Detail::DatePart part) const {
		using namespace Detail;

		int iDays = GetTotalDays();					// �� �ϼ��� ������. ex) 2022���̸� ���� �����̸� 52��ġ �ϼ��� ������

		int i4Years = iDays / Detail::DaysPer4Years_v;		// �� �ϼ����� 4���� ���� ���Ѵ�.
		iDays -= Detail::DaysPer4Years_v * i4Years;			// 4���� ���� ���ش�.

		int i1Years = iDays / Detail::DaysPer1Years_v;		// ���� �ϼ����� 1���� ���� ���Ѵ�.
		iDays -= Detail::DaysPer1Years_v * i1Years;			// ������� ��� �Ұŵǰ� 366�� �̳��� ���� ����

		// 1���� 365.2545���̹Ƿ� 366�� °�� 1���� ���� ��� ������
		if (i1Years == 4)
			i1Years = 3;

		// 4��, �� ���������� 366���̹Ƿ�
		int* pUntilDays = i1Years == 3 ? (int*)DaysUntilMonth366 : (int*)DaysUntilMonth365;
		int* pForDays = i1Years == 3 ? (int*)DaysForMonth366 : (int*)DaysForMonth365;

		if (part == DatePart::Year) {
			return 1970 + i4Years * 4 + i1Years;
		} else if (part == DatePart::DayOfYear) {
			return iDays + 1;
		} 

		int iMonth = 0;

		for (int i = 0; i < 13; i++) {
			if (iDays >= pUntilDays[i]) {
				iMonth = i; 
				break;
			}
		}

		if (part == DatePart::Month) {
			return iMonth + 1; // 1������ �����̹Ƿ�
		} else if (part == DatePart::Day) {
			return iDays - pUntilDays[iMonth] + 1;	// 1�Ϻ��� �����̹Ƿ�
		} else if (part == DatePart::MaxDayOfMonth) {
			return pForDays[iMonth];
		}

		return -1;
	}
	String Format(const char* fmt) const;
public:
	static DateTime Now(Detail::TimeStandard timeStandard = Detail::TimeStandard::Local);
	static Int16U TimeZoneBiasHour();
private:
	Int64U m_Tick = 0ULL;
};


} // namespace JCore