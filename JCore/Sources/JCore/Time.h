/*
	�ۼ��� : ������
	������ �ð��� �ٷ� �� �ִ� ����� �߰��մϴ�.
	 - ����ũ���� ���� �ð� ���� ���
	 - C# �ð�/��¥ ���� ���� ���̺귯��(System.DateTime) ����
	 - C++ std::chrono ���� ���̺귯�� ����
	 
	�����ġ ���
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
		
			// 1ƽ�� 1����ũ����
			constexpr Int64U TicksPerMicroSecond	= 1;										// ����ũ���ʴ� �� ƽ���� 	1ƽ��					1����ũ����
			constexpr Int64U TicksPerMiliSecond		= TicksPerMicroSecond * MaxMicroSecond_v;	// �и��ʴ� �� ƽ����		1000ƽ��					1�и���
			constexpr Int64U TicksPerSecond			= TicksPerMiliSecond * MaxMiliSecond_v;		// �ʴ� �� ƽ����			1000000ƽ��				1��
			constexpr Int64U TicksPerMinute			= TicksPerSecond * MaxSecond_v;				// 1�д� �� ƽ����		1000000 * 60			1��
			constexpr Int64U TicksPerHour			= TicksPerMinute * MaxMinute_v;				// 1�ð��� �� ƽ����		1000000 * 60 * 60		1�ð�
			constexpr Int64U TicksPerDay			= TicksPerHour * MaxHour_v;					// 1�ϴ� �� ƽ����		1000000 * 60 * 60 * 24	1��

			constexpr int DaysPer1Years_v = MaxDay_v;
			constexpr int DaysPer4Years_v = MaxDay_v * 4 + 1;				// 4���� �������� : 1461��
			constexpr int DaysPer100Years_v = DaysPer4Years_v * 25 - 1;		// 100���� �������� : 36524��
			constexpr int DaysPer400Years_v = DaysPer100Years_v * 4 + 1;	// 400���� �������� : 146097��
		
			constexpr int DaysUntilMonth365[13] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
			constexpr int DaysUntilMonth366[13] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

			constexpr int DaysForMonth365[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
			constexpr int DaysForMonth366[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };



			/*
			   After Christ 1970�⵵������ �ð��� ���Ѵ����� �غôµ� ������ ����;
			   �Ʒ� ó�� �� ���� �Ҽ��� �������� ����� �ȵȴ�.
			   constexpr long double EpochTimeBeginAC = long double(DaysPer400Years_v * 1969) / 400; // �� ���� �Ҽ��� ������ ��������
			*/
		
			// [AD 0001�� 1�� 1�� ~ 1969�� 12�� 31�ϱ����� ����ũ���� ���]
			constexpr Int64U ADBegin	 = (DaysPer400Years_v * 1969) / 400;	// Epoch �ð� 1970�� 1�� 1�ϱ����� �� ��
			constexpr Int64U ADBeginTick = ADBegin * TicksPerDay;				// Epoch �ð� 1970�� 1�� 1�ϱ����� ����ũ����
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

		enum class TimeStandard
		{
			Local,			// ���� ��ǻ�� �ý��� ���� �ð�
			Universal		// UTC ���ؽð�
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

			���ǻ��� : �ð����� Add ���� ��� ������ �������մϴ�.
			          Subtract �Լ��� ������ּ���.
		   =================================================================================== */
		struct Date
		{
			Int16U Year;
			Int16U Month;
			Int16U Day;

			Date() = default;
			Date(Int16U year, Int16U month, Int16U day) : Year(year), Month(month), Day(day) {}

			virtual void AddYear(Int32U years);
			virtual void AddMonth(Int32U months);
			virtual void AddDay(Int32U days);

			Int64U ToTick();
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

			virtual void AddHour(Int64U hours);
			virtual void AddMinute(Int64U minutes);
			virtual void AddSecond(Int64U seconds);
			virtual void AddMiliSecond(Int64U miliSeconds);
			virtual void AddMicroSecond(Int64U microSeconds, TimeUnit timeUnit = TimeUnit::MicroSecond);

			Int64U ToTick();
		};

		struct DateTime;
		struct DateAndTime : Date, Time {
			DateAndTime() = default;
			DateAndTime(Int16U year, Int16U month, Int16U day, Int16U hour, Int16U minute, Int16U second, Int16U miliSecond, Int16U microSecond)
				: Date(year, month, day), Time(hour, minute, second, miliSecond, microSecond)
			{}

			void AddYear(Int32U years) override;				// Date::AddYear�� ����
			void AddMonth(Int32U months) override;				// Date::AddMonth�� ����
			void AddDay(Int32U days) override;					// ��� �ٸ�
			void AddHour(Int64U hours) override;				// Time::AddHour�� ����
			void AddMinute(Int64U minutes) override;			// Time::AddMinute�� ����
			void AddSecond(Int64U seconds) override;			// Time::AddSecond�� ����
			void AddMiliSecond(Int64U miliSeconds) override;	// Time::AddMiliSecond�� ����
			void AddMicroSecond(Int64U microSeconds, TimeUnit timeUnit = TimeUnit::MicroSecond) override;	// ��� �ٸ�

			DateTime ToDateTime();
			Int64U ToTick();
		};

		class String;
		class DateTime
		{
		public: // constructors
			constexpr DateTime() : m_Tick(0) {}
			constexpr DateTime(Int64U tick) : m_Tick(tick) {}

		public: // public non-static
			// Ư�� Ÿ�����ֿ� �ش��ϴ� ��ü�ð� ���
			inline constexpr Int64U GetTick() const { return m_Tick; }
			inline constexpr Int64U GetTotalDays() const { return m_Tick / Detail::TicksPerDay; }
			inline constexpr Int64U GetTotalHours() const { return m_Tick / Detail::TicksPerHour; }
			inline constexpr Int64U GetTotalMinutes() const { return m_Tick / Detail::TicksPerMinute; }
			inline constexpr Int64U GetTotalSeconds() const { return m_Tick / Detail::TicksPerSecond; }
			inline constexpr Int64U GetTotalMiliSeconds() const { return m_Tick / Detail::TicksPerMiliSecond; }
			inline constexpr Int64U GetTotalMicroSeconds() const { return m_Tick; }

			// Ư�� Ÿ�����ֺ��� �ð� ���
			inline constexpr int GetYear() const { return GetDatePart(DatePart::Year); }
			inline constexpr int GetMonth() const { return GetDatePart(DatePart::Month); }
			inline constexpr int GetDay() const { return GetDatePart(DatePart::Day); }
			inline constexpr int GetHour() const { return GetTotalHours() % Detail::MaxHour_v; }
			inline constexpr int GetMinute() const { return GetTotalMinutes() % Detail::MaxMinute_v; }
			inline constexpr int GetSecond() const { return GetTotalSeconds() % Detail::MaxSecond_v; }
			inline constexpr int GetMiliSecond() const { return GetTotalMiliSeconds() % Detail::MaxMiliSecond_v; }
			inline constexpr int GetMicroSecond() const { return GetTotalMicroSeconds() % Detail::MaxMicroSecond_v; }

			// ���� ��������
			inline constexpr DayOfWeek GetDayOfWeek() const { return DayOfWeek(GetTotalDays() % 7 + 1); }

			// �ϳ�������� ��������
			inline constexpr int GetDayOfYear() const { return GetDatePart(DatePart::DayOfYear); }

			// �̹����� �ִ� ��������
			inline constexpr int GetMaxDayOfMonth() const { return GetDatePart(DatePart::MaxDayOfMonth); }

			// Ÿ�� ��ȯ
			inline DateAndTime ToDateAndTime() const {
				return DateAndTime(Int16U(GetYear()), Int16U(GetMonth()), Int16U(GetDay()),	// Date ����
								   Int16U(GetHour()), Int16U(GetMinute()), Int16U(GetSecond()), Int16U(GetMiliSecond()), Int16U(GetMicroSecond())); // Time ����
			}

			inline Date ToDate() const {
				return Date(Int16U(GetYear()), Int16U(GetMonth()), Int16U(GetDay()));
			}
			inline Time ToTime() const {
				return Time(Int16U(GetHour()), Int16U(GetMinute()), Int16U(GetSecond()), Int16U(GetMiliSecond()), Int16U(GetMicroSecond()));
			}

			// �ð� ����
			void AddYear(Int32U year);
			void AddMonth(Int32U month);
			void AddDay(Int32U day) { m_Tick += day * Detail::TicksPerDay; }
			void AddHour(Int32U hour) { m_Tick += hour * Detail::TicksPerHour; }
			void AddMinute(Int32U minute) { m_Tick += minute * Detail::TicksPerMinute; }
			void AddSecond(Int32U second) { m_Tick += second * Detail::TicksPerSecond; }
			void AddMiliSecond(Int32U miliSecond) { m_Tick += miliSecond * Detail::TicksPerMiliSecond; }

			void SubtractDay(Int32U day) { m_Tick -= day * Detail::TicksPerDay; }
			void SubtractHour(Int32U hour) { m_Tick -= hour * Detail::TicksPerHour; }
			void SubtractMinute(Int32U minute) { m_Tick -= minute * Detail::TicksPerMinute; }
			void SubtractAddSecond(Int32U second) { m_Tick -= second * Detail::TicksPerSecond; }
			void SubtractMiliSecond(Int32U miliSecond) { m_Tick -= miliSecond * Detail::TicksPerMiliSecond; }

			JCore::String Format(const char* fmt) const;

		private: // private static
			
			// ���� �⵵���� �ϼ��� ������
			static constexpr auto GetYearsFromDays(int days) {
				using namespace Detail;

				int i400Years = days / DaysPer400Years_v;	// �� �ϼ����� 400���� ���� ���Ѵ�.
				days -= DaysPer400Years_v * i400Years;		// 400���� ���� ���ش�.

				int i100Years = days / DaysPer100Years_v;	// �� �ϼ����� 100���� ���� ���Ѵ�.
				days -= DaysPer100Years_v * i100Years;		// 100���� ���� ���ش�.

				int i4Years = days / DaysPer4Years_v;		// �� �ϼ����� 4���� ���� ���Ѵ�.
				days -= DaysPer4Years_v * i4Years;			// 4���� ���� ���ش�.

				int i1Years = days / DaysPer1Years_v;		// ���� �ϼ����� 1���� ���� ���Ѵ�.
				days -= DaysPer1Years_v * i1Years;			// ������� ��� �Ұŵǰ� 366�� �̳��� ���� ����

				return MakeTuple(i400Years, i100Years, i4Years, i1Years, days);
			}
		private: // private non-static
			constexpr int GetDatePart(const DatePart part) const {
				using namespace Detail;

				auto years = GetYearsFromDays(GetTotalDays());

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
		public: // public static
			static DateTime Now(TimeStandard timeStandard = TimeStandard::Local);
			static Int16U TimeZoneBiasHour();

			// @���� ���� ���� : https://ko.wikipedia.org/wiki/%EC%9C%A4%EB%85%84 
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

		
		private:
			Int64U m_Tick;

			friend struct DateAndTime;
			friend struct Date;
			friend struct Time;
		};

	} // namespace Time
} // namespace JCore





/*
 ���ø��� �ڵ尡 ���������� ����� ������ �ֱ� ������ ��������..
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