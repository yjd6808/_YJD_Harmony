/*
	�ۼ��� : ������
	������ �ð��� �ٷ� �� �ִ� ����� �߰��մϴ�.
	 - 100�� �ڸ� ���� ������ ���е��� �ð����� ���
	 - C# �ð�/��¥ ���� ���� ���̺귯��(System.DateTime) ����
	 - C++ std::chrono ���� ���̺귯�� ����

	�����ġ ���
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

			// ���� 1�� 31�ϳ� 13������ ���ߴٰ� ���� �� 
			// �� ������ �����ع����� 2�� 31���� �Ǿ������. (2���� ���������� ������ ��� 29�� ����� ��� 28�� ��)
			// �׷��� ����/��⿡ ���� �ش� ���� �ִ��� ���� �ʰ��ϴ� ��� �ִ� �ϼ��� �����������
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

		// ���˿� ���� �ð� ���ڿ��� �򵵷� �Ѵ�.
		JCore::String DateTime::Format(const char* fmt) const {
			JCore::String szFmt(fmt);
			return "";
		}

		// Ÿ���� �ð� ���� ��� �Լ�
		// @���� : https://docs.microsoft.com/ko-kr/windows/win32/api/timezoneapi/nf-timezoneapi-gettimezoneinformation?redirectedfrom=MSDN (Ÿ���� ���� ��¹�)
		Int16U DateTime::TimeZoneBiasHour() {
			TIME_ZONE_INFORMATION timeZoneInformation;
			GetTimeZoneInformation(&timeZoneInformation);
			return (timeZoneInformation.Bias * -1) / MaxMinute_v;
		}

		// ����ð� ��� �Լ�
		// @���� : https://stackoverflow.com/questions/15957805/extract-year-month-day-etc-from-stdchronotime-point-in-c (����ð� ��� ���)
		// @���� : https://stackoverflow.com/questions/32811729/add-time-duration-to-c-timepoint (�ð� ������)
		DateTime DateTime::Now(TimeStandard timeStandard) {
			chrono::system_clock::time_point now = chrono::system_clock::now();

			// Unix Epoch UTC �ð��� ����ũ���� ������ ���Ѵ�.
			//  = 1970�� 1�� 1�Ϻ��� ��������� ����ũ����
			Int64U epoch = chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()).count(); 

			// After Christ (0001�� 1�� 1�� ~ 1969�� 12�� 31��)������ ����ũ���ʸ� ���������ν�
			// 0001�� 1�� 1�� ~ ��������� After Chirst UTC �ð��� ���Ѵ�.
			epoch += ADBeginTick;

			// ���� �ð��� Ÿ���� ������ŭ �����ش�.
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

