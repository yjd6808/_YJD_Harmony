/*
	작성자 : 윤정도
	간단한 시간을 다룰 수 있는 기능을 추가합니다.
	 - 100의 자리 나노 세컨드 정밀도의 시간관련 기능 
	 - C# 시간/날짜 관련 내장 라이브러리(System.DateTime) 참고
	 - C++ std::chrono 내장 라이브러리 참고
	 
	스톱워치 기능
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
		
		// 1틱당 1나노초
		constexpr Int64U TicksPerMicroSecond	= MaxNanoSecond_v;							// 마이크로초당 몇 틱인지 	1000틱당						1마이크로초
		constexpr Int64U TicksPerMiliSecond		= TicksPerMicroSecond * MaxMicroSecond_v;	// 밀리초당 몇 틱인지		1000000틱당					1밀리초
		constexpr Int64U TicksPerSecond			= TicksPerMiliSecond * MaxMiliSecond_v;		// 초당 몇 틱인지			1000000000틱당				1초
		constexpr Int64U TicksPerMinute			= TicksPerSecond * MaxSecond_v;				// 1분당 몇 틱인지		1000000000 * 60				1분
		constexpr Int64U TicksPerHour			= TicksPerMinute * MaxMinute_v;				// 1시간당 몇 틱인지		1000000000 * 60 * 60		1시간
		constexpr Int64U TicksPerDay			= TicksPerHour * MaxHour_v;					// 1일당 몇 틱인지		1000000000 * 60 * 60 * 24	1일

		constexpr int DaysPer1Years_v = MaxDay_v;
		constexpr int DaysPer4Years_v = MaxDay_v * 4 + 1;				// 4년이 몇일인지 : 1461일
		constexpr int DaysPer100Years_v = DaysPer4Years_v * 25 - 1;		// 100년이 몇일인지 : 36524일
		constexpr int DaysPer400Years_v = DaysPer100Years_v * 4 + 1;	// 400년이 몇일인지 : 146097일
		
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

		enum class TimeStandard
		{
			Local,			// 현재 컴퓨터 시스템 기준 시간
			Universal		// UTC 기준시간
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
	// 특정 타입유닛에 해당하는 전체시간 얻기
	inline constexpr Int64U GetTick() const { return m_Tick; }
	inline constexpr Int64U GetTotalDays() const { return m_Tick / Detail::TicksPerDay; }
	inline constexpr Int64U GetTotalHours() const { return m_Tick / Detail::TicksPerHour; }
	inline constexpr Int64U GetTotalMinutes() const { return m_Tick / Detail::TicksPerMinute; }
	inline constexpr Int64U GetTotalSeconds() const { return m_Tick / Detail::TicksPerSecond; }
	inline constexpr Int64U GetTotalMiliSeconds() const { return m_Tick / Detail::TicksPerMiliSecond; }
	inline constexpr Int64U GetTotalMicroSeconds() const { return m_Tick / Detail::TicksPerMicroSecond; }
	inline constexpr Int64U GetTotalNanoSeconds() const { return m_Tick; }

	// 특정 타입유닛별로 시간 얻기
	inline constexpr int GetYear() const { return GetDatePart(Detail::DatePart::Year); }
	inline constexpr int GetMonth() const { return GetDatePart(Detail::DatePart::Month); }
	inline constexpr int GetDay() const { return GetDatePart(Detail::DatePart::Day); }
	inline constexpr int GetHour() const { return GetTotalHours() % Detail::MaxHour_v; }
	inline constexpr int GetMinute() const { return GetTotalMinutes() % Detail::MaxMinute_v; }
	inline constexpr int GetSecond() const { return GetTotalSeconds() % Detail::MaxSecond_v; }
	inline constexpr int GetMiliSecond() const { return GetTotalMiliSeconds() % Detail::MaxMiliSecond_v; }
	inline constexpr int GetMicroSecond() const { return GetTotalMicroSeconds() % Detail::MaxMicroSecond_v; }
	inline constexpr int GetNanoSecond() const { return m_Tick % Detail::MaxNanoSecond_v; }

	// 무슨 요일인지
	inline constexpr Detail::DayOfWeek GetDayOfWeek() const { return Detail::DayOfWeek(GetTotalDays() / 7 + 1); }

	// 일년기준으로 몇일인지
	inline constexpr int GetDayOfYear() const { return GetDatePart(Detail::DatePart::DayOfYear); }

	// 이번달이 최대 몇일인지
	inline constexpr int GetMaxDayOfMonth() const { return GetDatePart(Detail::DatePart::MaxDayOfMonth); }

	// 타입 변환
	inline constexpr DateAndTime ToDateAndTime() const {
		return { Int16U(GetYear()), Int16U(GetMonth()),	 Int16U(GetDay()),	// Date 정보
				 Int16U(GetHour()), Int16U(GetMinute()), Int16U(GetSecond()), Int16U(GetMiliSecond()), Int16U(GetMicroSecond()), Int16U(GetNanoSecond()) };	// Time 정보
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

		int iDays = GetTotalDays();					// 총 일수를 가져옴. ex) 2022년이면 오늘 기준이면 52년치 일수를 가져옴

		int i4Years = iDays / Detail::DaysPer4Years_v;		// 총 일수에서 4년의 수를 구한다.
		iDays -= Detail::DaysPer4Years_v * i4Years;			// 4년의 수를 빼준다.

		int i1Years = iDays / Detail::DaysPer1Years_v;		// 남은 일수에서 1년의 수를 구한다.
		iDays -= Detail::DaysPer1Years_v * i1Years;			// 년단위는 모두 소거되고 366일 이내의 값이 남음

		// 1년이 365.2545일이므로 366일 째에 1일이 남는 경우 때문에
		if (i1Years == 4)
			i1Years = 3;

		// 4년, 즉 마지막년은 366일이므로
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
			return iMonth + 1; // 1월부터 시작이므로
		} else if (part == DatePart::Day) {
			return iDays - pUntilDays[iMonth] + 1;	// 1일부터 시작이므로
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