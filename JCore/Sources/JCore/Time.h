/*
	작성자 : 윤정도
	간단한 시간을 다룰 수 있는 기능을 추가합니다.
	스톱워치 기능
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/Tuple.h>

namespace JCore {
	namespace Time {
		namespace Detail {
			constexpr Int64U MaxDay_v			= 365;
			constexpr Int64U MaxHour_v			= 24;
			constexpr Int64U MaxMinute_v		= 60;
			constexpr Int64U MaxSecond_v		= 60;
			constexpr Int64U MaxMiliSecond_v	= 1000;
			constexpr Int64U MaxMicroSecond_v	= 1000;
		
			// 1틱당 1마이크로초
			constexpr Int64U TicksPerMicroSecond	= 1;										// 마이크로초당 몇 틱인지 	1틱당					1마이크로초
			constexpr Int64U TicksPerMiliSecond		= TicksPerMicroSecond * MaxMicroSecond_v;	// 밀리초당 몇 틱인지		1000틱당					1밀리초
			constexpr Int64U TicksPerSecond			= TicksPerMiliSecond * MaxMiliSecond_v;		// 초당 몇 틱인지			1000000틱당				1초
			constexpr Int64U TicksPerMinute			= TicksPerSecond * MaxSecond_v;				// 1분당 몇 틱인지		1000000 * 60			1분
			constexpr Int64U TicksPerHour			= TicksPerMinute * MaxMinute_v;				// 1시간당 몇 틱인지		1000000 * 60 * 60		1시간
			constexpr Int64U TicksPerDay			= TicksPerHour * MaxHour_v;					// 1일당 몇 틱인지		1000000 * 60 * 60 * 24	1일

			constexpr int DaysPer1Years_v = MaxDay_v;
			constexpr int DaysPer4Years_v = MaxDay_v * 4 + 1;				// 4년이 몇일인지 : 1461일
			constexpr int DaysPer100Years_v = DaysPer4Years_v * 25 - 1;		// 100년이 몇일인지 : 36524일
			constexpr int DaysPer400Years_v = DaysPer100Years_v * 4 + 1;	// 400년이 몇일인지 : 146097일
		
			constexpr int DaysUntilMonth365[13] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
			constexpr int DaysUntilMonth366[13] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

			constexpr int DaysForMonth365[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
			constexpr int DaysForMonth366[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };



			/*
			   After Christ 1970년도까지의 시간을 구한다음에 해봤는데 오차가 생김;
			   아래 처럼 일 수의 소수점 단위까지 들고가면 안된다.
			   constexpr long double EpochTimeBeginAC = long double(DaysPer400Years_v * 1969) / 400; // 일 수의 소수점 단위는 버려야함
			*/
		
			// [AD 0001년 1월 1일 ~ 1969년 12월 31일까지의 마이크로초 계산]
			constexpr Int64U ADBegin	 = (DaysPer400Years_v * 1969) / 400;	// Epoch 시간 1970년 1월 1일까지의 일 수
			constexpr Int64U ADBeginTick = ADBegin * TicksPerDay;				// Epoch 시간 1970년 1월 1일까지의 마이크로초
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
		   =================================================================================== */
		struct Date
		{
			Int16U Year;
			Int16U Month;
			Int16U Day;

			Date() = default;
			Date(Int16U year, Int16U month, Int16U day) : Year(year), Month(month), Day(day) {}

			virtual void AddYear(Int16 years);
			virtual void AddMonth(Int16 months);
			virtual void AddDay(Int16 days);

			virtual void SubtractYear(Int16 years);
			virtual void SubtractMonth(Int16 years);
			virtual void SubtractDay(Int16 years);

			Int64U ToTick() const;

		protected:
			static Date ConvertTickToDate(Int64U tick);
		};

		struct Time
		{
			Int16U Hour;
			Int16U Minute;
			Int16U Second;
			Int16U MiliSecond;
			Int16U MicroSecond;

			Time() = default;
			Time(Int16U hour, Int16U minute, Int16U second, Int16U miliSecond, Int16U microSecond)
				: Hour(hour), Minute(minute), Second(second), MiliSecond(miliSecond), MicroSecond(microSecond)
			{}

			virtual void AddHour(Int64 hours);
			virtual void AddMinute(Int64 minutes);
			virtual void AddSecond(Int64 seconds);
			virtual void AddMiliSecond(Int64 miliSeconds);
			virtual void AddMicroSecond(Int64 microSeconds, TimeUnit timeUnit = TimeUnit::MicroSecond);

			virtual void SubtractHour(Int64 hours);
			virtual void SubtractMinute(Int64 minutes);
			virtual void SubtractSecond(Int64 seconds);
			virtual void SubtractiliSecond(Int64 miliSeconds);
			virtual void SubtractMicroSecond(Int64 microSeconds, TimeUnit timeUnit = TimeUnit::MicroSecond);

			Int64U ToTick() const;
		};

		struct DateTime;
		struct DateAndTime : Date, Time {
			DateAndTime() = default;
			DateAndTime(Int16U year, Int16U month, Int16U day, Int16U hour, Int16U minute, Int16U second, Int16U miliSecond, Int16U microSecond)
				: Date(year, month, day), Time(hour, minute, second, miliSecond, microSecond)
			{}

			void AddYear(Int16 years) override;				// Date::AddYear와 동일
			void AddMonth(Int16 months) override;			// Date::AddMonth와 동일
			void AddDay(Int16 days) override;				
			void AddHour(Int64 hours) override;				
			void AddMinute(Int64 minutes) override;			
			void AddSecond(Int64 seconds) override;			
			void AddMiliSecond(Int64 miliSeconds) override;	
			void AddMicroSecond(Int64 microSeconds, TimeUnit timeUnit = TimeUnit::MicroSecond) override;

			void SubtractYear(Int16 years) override;
			void SubtractMonth(Int16 years) override;
			void SubtractDay(Int16 years) override;
			void SubtractHour(Int64 hours) override;
			void SubtractMinute(Int64 minutes) override;
			void SubtractSecond(Int64 seconds) override;
			void SubtractiliSecond(Int64 miliSeconds) override;
			void SubtractMicroSecond(Int64 microSeconds, TimeUnit timeUnit = TimeUnit::MicroSecond) override;


			DateTime ToDateTime() const;
			Int64U ToTick() const;
		};

		class JCore::String;
		class DateTime
		{
		public: // constructors
			constexpr DateTime() : m_Tick(0) {}
			constexpr DateTime(Int64U tick) : m_Tick(tick) {}

		public: // public non-static
			// 특정 타입유닛에 해당하는 전체시간 얻기
			inline constexpr Int64U GetTick() const { return m_Tick; }
			inline constexpr Int64U GetTotalDays() const { return m_Tick / Detail::TicksPerDay; }
			inline constexpr Int64U GetTotalHours() const { return m_Tick / Detail::TicksPerHour; }
			inline constexpr Int64U GetTotalMinutes() const { return m_Tick / Detail::TicksPerMinute; }
			inline constexpr Int64U GetTotalSeconds() const { return m_Tick / Detail::TicksPerSecond; }
			inline constexpr Int64U GetTotalMiliSeconds() const { return m_Tick / Detail::TicksPerMiliSecond; }
			inline constexpr Int64U GetTotalMicroSeconds() const { return m_Tick; }

			// 특정 타입유닛별로 시간 얻기
			inline constexpr int GetYear() const { return GetDatePart(DatePart::Year); }
			inline constexpr int GetMonth() const { return GetDatePart(DatePart::Month); }
			inline constexpr int GetDay() const { return GetDatePart(DatePart::Day); }
			inline constexpr int GetHour() const { return GetTotalHours() % Detail::MaxHour_v; }
			inline constexpr int GetMinute() const { return GetTotalMinutes() % Detail::MaxMinute_v; }
			inline constexpr int GetSecond() const { return GetTotalSeconds() % Detail::MaxSecond_v; }
			inline constexpr int GetMiliSecond() const { return GetTotalMiliSeconds() % Detail::MaxMiliSecond_v; }
			inline constexpr int GetMicroSecond() const { return GetTotalMicroSeconds() % Detail::MaxMicroSecond_v; }

			// 무슨 요일인지
			inline constexpr DayOfWeek GetDayOfWeek() const { return DayOfWeek(GetTotalDays() % 7); }

			// 몇달인지
			inline constexpr MonthOfYear GetMonthEnum() const { return MonthOfYear(GetMonth() - 1); }

			// 일년기준으로 몇일인지
			inline constexpr int GetDayOfYear() const { return GetDatePart(DatePart::DayOfYear); }

			// 이번달이 최대 몇일인지
			inline constexpr int GetMaxDayOfMonth() const { return GetDatePart(DatePart::MaxDayOfMonth); }

			// 오전인지 오후인지
			inline constexpr AMPM GetAMPM() const { return (GetHour() / 12) > 0 ? AMPM::PM : AMPM::AM; }

			// 타입 변환
			inline DateAndTime ToDateAndTime() const {
				return DateAndTime(Int16U(GetYear()), Int16U(GetMonth()), Int16U(GetDay()),	// Date 정보
								   Int16U(GetHour()), Int16U(GetMinute()), Int16U(GetSecond()), Int16U(GetMiliSecond()), Int16U(GetMicroSecond())); // Time 정보
			}

			inline Date ToDate() const {
				return Date(Int16U(GetYear()), Int16U(GetMonth()), Int16U(GetDay()));
			}
			inline Time ToTime() const {
				return Time(Int16U(GetHour()), Int16U(GetMinute()), Int16U(GetSecond()), Int16U(GetMiliSecond()), Int16U(GetMicroSecond()));
			}

			// 시간 연산
			void AddYear(Int32 year);
			void AddMonth(Int32 month);
			void AddDay(Int32 day) { m_Tick += day * Detail::TicksPerDay; }
			void AddHour(Int32 hour) { m_Tick += hour * Detail::TicksPerHour; }
			void AddMinute(Int32 minute) { m_Tick += minute * Detail::TicksPerMinute; }
			void AddSecond(Int32 second) { m_Tick += second * Detail::TicksPerSecond; }
			void AddMiliSecond(Int32 miliSecond) { m_Tick += miliSecond * Detail::TicksPerMiliSecond; }

			void SubtractYear(Int32 year);
			void SubtractMonth(Int32 month);
			void SubtractDay(Int32 day) { m_Tick -= day * Detail::TicksPerDay; }
			void SubtractHour(Int32 hour) { m_Tick -= hour * Detail::TicksPerHour; }
			void SubtractMinute(Int32 minute) { m_Tick -= minute * Detail::TicksPerMinute; }
			void SubtractAddSecond(Int32 second) { m_Tick -= second * Detail::TicksPerSecond; }
			void SubtractMiliSecond(Int32 miliSecond) { m_Tick -= miliSecond * Detail::TicksPerMiliSecond; }

			String Format(const char* fmt) const;

		private: // private static
			// 단위 년도별로 일수를 가져옴
			static constexpr auto GetYearsFromDays(int days) {
				using namespace Detail;

				int i400Years = days / DaysPer400Years_v;	// 총 일수에서 400년의 수를 구한다.
				days -= DaysPer400Years_v * i400Years;		// 400년의 수를 빼준다.

				int i100Years = days / DaysPer100Years_v;	// 총 일수에서 100년의 수를 구한다.
				days -= DaysPer100Years_v * i100Years;		// 100년의 수를 빼준다.

				int i4Years = days / DaysPer4Years_v;		// 총 일수에서 4년의 수를 구한다.
				days -= DaysPer4Years_v * i4Years;			// 4년의 수를 빼준다.

				int i1Years = days / DaysPer1Years_v;		// 남은 일수에서 1년의 수를 구한다.
				days -= DaysPer1Years_v * i1Years;			// 년단위는 모두 소거되고 366일 이내의 값이 남음

				return MakeTuple(i400Years, i100Years, i4Years, i1Years, days);
			}

			static constexpr int GetDatePart(const Int64U tick, const DatePart part) {
				using namespace Detail;

				DateTime dateTime(tick);

				auto years = GetYearsFromDays(dateTime.GetTotalDays());

				int i400Years = years.item1;
				int i100Years = years.item2;
				int i4Years = years.item3;
				int i1Years = years.item4;
				int iLeftDays = years.item5;

				// 1년이 365.2545일이므로 366일 째에 1일이 남는 경우 때문에
				if (i1Years == 4)
					i1Years = 3;

				// 4년, 즉 마지막년은 366일이므로
				int* pUntilDays = i1Years == 3 ? (int*)DaysUntilMonth366 : (int*)DaysUntilMonth365;
				int* pForDays = i1Years == 3 ? (int*)DaysForMonth366 : (int*)DaysForMonth365;


				if (part == DatePart::Year) {
					return i400Years * 400 + i100Years * 100 + i4Years * 4 + i1Years + 1;	// 년도도 1년부터 시작이기 때문에 + 1
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
					return iMonth + 1; // 1월부터 시작이므로
				} else if (part == DatePart::Day) {
					return iLeftDays - pUntilDays[iMonth] + 1;	// 1일부터 시작이므로
				} else if (part == DatePart::MaxDayOfMonth) {
					return pForDays[iMonth];
				}

				return -1;
			}
		private: // private non-static
			inline constexpr int GetDatePart(const DatePart part) const {
				return DateTime::GetDatePart(m_Tick, part);
			}

			void ReflectFormat(const DateAndTime& time, String& ret, char token, int count) const;

		public: // public static
			static DateTime Now(TimeStandard timeStandard = TimeStandard::Local);
			static Int16 TimeZoneBiasMinute();

			// @윤년 조건 참고 : https://ko.wikipedia.org/wiki/%EC%9C%A4%EB%85%84 
			static constexpr bool IsLeapYear(int year) {
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
			Int64U m_Tick;

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

	} // namespace Time
} // namespace JCore





/*
 템플릿은 코드가 복잡해지게 만드는 단점이 있기 때문에 쓰지말자..
template <int Value>
struct DayUnit
{
	static constexpr Int64U Days = Value;
	static constexpr Int64U Hours = Detail::MaxHour_v * Value;
	static constexpr Int64U Minutes = Hours * Detail::MaxMinute_v;
	static constexpr Int64U Seconds = Minutes * Detail::MaxSecond_v;
	static constexpr Int64U MiliSeconds = Seconds * Detail::MaxMiliSecond_v;
	static constexpr Int64U MicroSeconds = MiliSeconds * Detail::MaxMicroSecond_v;
};

template <int Value>
struct HourUnit
{
	static constexpr Int64U Hours = Value;
	static constexpr Int64U Minutes = Hours * Detail::MaxMinute_v;
	static constexpr Int64U Seconds = Minutes * Detail::MaxSecond_v;
	static constexpr Int64U MiliSeconds = Seconds * Detail::MaxMiliSecond_v;
	static constexpr Int64U MicroSeconds = MiliSeconds * Detail::MaxMicroSecond_v;
};

template <int Value>
struct MinuteUnit
{
	static constexpr Int64U Minutes = Value;
	static constexpr Int64U Seconds = Minutes * Detail::MaxSecond_v;
	static constexpr Int64U MiliSeconds = Seconds * Detail::MaxMiliSecond_v;
	static constexpr Int64U MicroSeconds = MiliSeconds * Detail::MaxMicroSecond_v;
};

template <int Value>
struct SecondUnit
{
	static constexpr Int64U Seconds = Value;
	static constexpr Int64U MiliSeconds = Seconds * Detail::MaxMiliSecond_v;
	static constexpr Int64U MicroSeconds = MiliSeconds * Detail::MaxMicroSecond_v;
};

template <int Value>
struct MiliSecondUnit
{
	static constexpr Int64U MiliSeconds = Value;
	static constexpr Int64U MicroSeconds = MiliSeconds * Detail::MaxMicroSecond_v;
};

template <int Value>
struct MicroSecondUnit
{
	static constexpr Int64U MicroSeconds = Value;
};*/