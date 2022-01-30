/*
	작성자 : 윤정도
	간단한 시간을 다룰 수 있는 기능을 추가합니다.
	 - 100의 자리 나노 세컨드 정밀도의 시간관련 기능
	 - C# 시간/날짜 관련 내장 라이브러리(System.DateTime) 참고
	 - C++ std::chrono 내장 라이브러리 참고

	스톱워치 기능
*/

#include <JCore/Core.h>
#include <JCore/Time.h>
#include <JCore/String.h>

#include <chrono>
#include <timezoneapi.h>

using namespace std;

namespace JCore {
	namespace Time {

		using namespace Detail;

		/*=====================================================================================
												Date
		 =====================================================================================*/

		void Date::AddYear(Int32U years) {
			AddMonth(years * 12);
		}

		void Date::AddMonth(Int32U months) {
			Month += months;
			Year += (Month - 1) / 12;
			Month %= 12;

			// 만약 1월 31일날 13개월을 더했다고 했을 때 
			// 달 단위만 변경해버리면 2월 31일이 되어버린다. (2월은 마지막일이 윤년일 경우 29일 평년일 경우 28일 임)
			// 그래서 윤년/평년에 따라 해당 달의 최대일 수를 초과하는 경우 최대 일수로 변경해줘야함
			if (DateTime::IsLeapYear(Year)) {
				Day = Day > DaysForMonth366[Month - 1] ? DaysForMonth366[Month - 1] : Day;
			} else {
				Day = Day > DaysForMonth365[Month - 1] ? DaysForMonth365[Month - 1] : Day;
			}
		}

		void Date::AddDay(Int32U day) {
			Date date = DateTime(ToTick() + day * TicksPerDay).ToDate();

			Year = date.Year;
			Month = date.Month;
			Day = date.Day;
		}

		Int64U Date::ToTick() {
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

			// 현재 계산된 달, 일은 0일, 0월부터 시작아니라 1일, 1월 부터 시작이기때문에 1씩 빼줘야한다.
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

		void Time::AddHour(Int64U hours) {
			AddMicroSecond(hours * TicksPerHour, TimeUnit::Hour);
		}

		void Time::AddMinute(Int64U minutes) {
			AddMicroSecond(minutes * TicksPerMinute, TimeUnit::Minute);
		}
		
		void Time::AddSecond(Int64U seconds) {
			AddMicroSecond(seconds * TicksPerSecond, TimeUnit::Second);
		}

		void Time::AddMiliSecond(Int64U miliSeconds) {
			AddMicroSecond(miliSeconds * TicksPerMiliSecond, TimeUnit::MiliSecond);
		}

		void Time::AddMicroSecond(Int64U microSeconds, TimeUnit timeUnit) {
			Int64U uiTick = ToTick() + microSeconds;

			Int64U uiHour = uiTick / TicksPerHour;
			Hour = uiHour % MaxHour_v;

			if (timeUnit == TimeUnit::Hour)
				return;

			Int64U uiMinute = uiTick / TicksPerMinute;
			Minute = uiMinute % MaxMinute_v;

			if (timeUnit == TimeUnit::Minute)
				return;

			Int64U uiSecond = uiTick / TicksPerSecond;
			Second = uiSecond % MaxSecond_v;

			if (timeUnit == TimeUnit::Second)
				return;

			Int64U uiMiliSecond = uiTick / TicksPerMiliSecond;
			MiliSecond = uiMiliSecond % MaxMiliSecond_v;

			if (timeUnit == TimeUnit::MiliSecond)
				return;


			MicroSecond = uiTick % MaxMicroSecond_v;
		}

		Int64U Time::ToTick() {
			Int64U uiTotalTick = 0;
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
		DateTime DateAndTime::ToDateTime() {
			return DateTime(ToTick());
		}

		Int64U DateAndTime::ToTick() {
			return Time::ToTick() + Date::ToTick();
		}
		
		void DateAndTime::AddYear(Int32U years) {
			AddMonth(years * 12);
		}

		void DateAndTime::AddMonth(Int32U months) {
			Date::AddMonth(months);
		}

		void DateAndTime::AddDay(Int32U days) {
			AddMicroSecond(days * TicksPerDay);
		}

		void DateAndTime::AddHour(Int64U hours) {
			Time::AddHour(hours);
		}

		void DateAndTime::AddMinute(Int64U minutes) {
			Time::AddMinute(minutes);
		}

		void DateAndTime::AddSecond(Int64U seconds) {
			Time::AddSecond(seconds);
		}

		void DateAndTime::AddMiliSecond(Int64U miliSeconds) {
			Time::AddMiliSecond(miliSeconds);
		}
		
		void DateAndTime::AddMicroSecond(Int64U microSeconds, TimeUnit timeUnit) {
			DateTime current(ToTick() + microSeconds);
			*this = current.ToDateAndTime();
		}

		/*=====================================================================================
											DateTime
		 =====================================================================================*/

		// 포맷에 따라 시간 문자열을 얻도록 한다.
		JCore::String DateTime::Format(const char* fmt) const {
			JCore::String szFmt(fmt);
			return "";
		}

		// 타임존 시간 편차 얻는 함수
		// @참고 : https://docs.microsoft.com/ko-kr/windows/win32/api/timezoneapi/nf-timezoneapi-gettimezoneinformation?redirectedfrom=MSDN (타임존 정보 얻는법)
		Int16U DateTime::TimeZoneBiasHour() {
			TIME_ZONE_INFORMATION timeZoneInformation;
			GetTimeZoneInformation(&timeZoneInformation);
			return (timeZoneInformation.Bias * -1) / MaxMinute_v;
		}

		// 현재시간 얻는 함수
		// @참고 : https://stackoverflow.com/questions/15957805/extract-year-month-day-etc-from-stdchronotime-point-in-c (현재시간 얻는 방법)
		// @참고 : https://stackoverflow.com/questions/32811729/add-time-duration-to-c-timepoint (시간 연산방법)
		DateTime DateTime::Now(TimeStandard timeStandard) {
			chrono::system_clock::time_point now = chrono::system_clock::now();

			// Unix Epoch UTC 시간을 마이크로초 단위로 구한다.
			//  = 1970년 1월 1일부터 현재까지의 마이크로초
			Int64U epoch = chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()).count(); 

			// After Christ (0001년 1월 1일 ~ 1969년 12월 31일)까지의 마이크로초를 더해줌으로써
			// 0001년 1월 1일 ~ 현재까지의 After Chirst UTC 시간을 구한다.
			epoch += ADBeginTick;

			// 로컬 시간은 타임존 편차만큼 더해준다.
			if (timeStandard == TimeStandard::Local) {
				Int16U uiBias = TimeZoneBiasHour();
				epoch += uiBias * TicksPerHour;
			}
			return DateTime(epoch);
		}

		void DateTime::AddYear(Int32U years) {
			AddMonth(years * 12);
		}

		void DateTime::AddMonth(Int32U months) {
			DateAndTime current = ToDateAndTime();
			current.AddMonth(months);
			m_Tick = current.ToTick();
		}

	} // namespace Time
} // namespace JCore

