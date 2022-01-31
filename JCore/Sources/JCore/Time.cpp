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

#include <unordered_map>
#include <chrono>
#include <timezoneapi.h>

using namespace std;

namespace JCore {
	namespace Time {

		using namespace Detail;

		/*=====================================================================================
												Date
		 =====================================================================================*/

		void Date::AddYear(Int16 years) {
			AddMonth(years * 12);
		}

		void Date::AddMonth(Int16 months) {
			if (months < 0) {
				SubtractMonth(months);
				return;
			}

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

		void Date::AddDay(Int16 day) {
			Date date = DateTime(ToTick() + day * TicksPerDay).ToDate();

			Year = date.Year;
			Month = date.Month;
			Day = date.Day;
		}


		void Date::SubtractYear(Int16 years) {
			SubtractYear(years * 12);
		}

		void Date::SubtractMonth(Int16 months) {
			if (months < 0) {
				AddMonth(Math::Abs(months));
				return;
			}

			Int16U iSubtractedYear = months / 12;
			Year -= iSubtractedYear;
			Month += iSubtractedYear * 12;
			Year += (Month - 1) / 12;
			Month %= 12;

			if (DateTime::IsLeapYear(Year)) {
				Day = Day > DaysForMonth366[Month - 1] ? DaysForMonth366[Month - 1] : Day;
			} else {
				Day = Day > DaysForMonth365[Month - 1] ? DaysForMonth365[Month - 1] : Day;
			}
		}
		void Date::SubtractDay(Int16 days) {
			if (days < 0) {
				AddDay(Math::Abs(days));
				return;
			}

			AddDay(days * -1);
		}


		Int64U Date::ToTick() const {
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

		void Time::SubtractHour(Int64 hours) {
			SubtractMicroSecond(hours * TicksPerHour, TimeUnit::Hour);
		}

		void Time::SubtractMinute(Int64 minutes) {
			SubtractMicroSecond(minutes * TicksPerMinute, TimeUnit::Minute);
		}

		void Time::SubtractSecond(Int64 seconds) {
			SubtractMicroSecond(seconds * TicksPerSecond, TimeUnit::Second);
		}

		void Time::SubtractiliSecond(Int64 miliSeconds) {
			SubtractMicroSecond(miliSeconds * TicksPerMiliSecond, TimeUnit::MiliSecond);
		}

		void Time::SubtractMicroSecond(Int64 microSeconds, TimeUnit timeUnit) {
			if (microSeconds < 0) {
				AddMicroSecond(Math::Abs(microSeconds));
				return;
			}

			AddMicroSecond(microSeconds * -1);
		}


		Int64U Time::ToTick() const {
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
		DateTime DateAndTime::ToDateTime() const {
			return DateTime(ToTick());
		}

		Int64U DateAndTime::ToTick() const {
			return Time::ToTick() + Date::ToTick();
		}
		
		void DateAndTime::AddYear(Int16 years) {
			Date::AddMonth(years * 12);
		}

		void DateAndTime::AddMonth(Int16 months) {
			Date::AddMonth(months);
		}

		void DateAndTime::AddDay(Int16 days) {
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
			DateTime current(ToTick() + microSeconds);
			*this = current.ToDateAndTime();
		}

		void DateAndTime::SubtractYear(Int16 years) {
			Date::SubtractMonth(years * 12);
		}
		void DateAndTime::SubtractMonth(Int16 months) {
			Date::SubtractMonth(months);
		}
		void DateAndTime::SubtractDay(Int16 days) {
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

		void DateAndTime::SubtractiliSecond(Int64 miliSeconds) {
			SubtractMicroSecond(miliSeconds * TicksPerMiliSecond);
		}

		void DateAndTime::SubtractMicroSecond(Int64 microSeconds, TimeUnit timeUnit) {
			DateTime current(ToTick() - microSeconds);
			*this = current.ToDateAndTime();
		}

		/*=====================================================================================
											DateTime
		 =====================================================================================*/

		const char* DateTime::ms_szWeekAbbrevName[] = {"Mon", "Tue" , "Wed" , "Thu" , "Fri" , "Sat", "Sun" };
		const char* DateTime::ms_szWeekFullName[] = { "Monday", "Tueday" , "Wednesday" , "Thursday" , "Friday" , "Saturday", "Sunday" };
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
		
		std::unordered_map<char, Tuple<char, DateFormat, int>, Hasher<char>> FormatTokenMap_v =
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
		
		void DateTime::ReflectFormat(const DateAndTime& time, JCore::String& ret, char token, int count) const {
			if (token == '\0' || count == 0) {
				return;
			}

			auto findIter = FormatTokenMap_v.find(token);
			if (findIter == FormatTokenMap_v.end()) {
				throw std::invalid_argument("�ùٸ��� ���� ���� ��ū�Դϴ�.");
			}

			auto tokenTuple = findIter->second;

			const DateFormat format = DateFormat(int(tokenTuple.item2) + count - 1);
			const int iMaxCount = tokenTuple.item3;	// �������� ���������� ���� �ִ� ��

			if (count > iMaxCount) {
				throw std::out_of_range("��ū ������ �̻��մϴ�. Ȯ���ϰ� �ٽ� �����ּ���.");
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
				Int16 timezonBias = TimeZoneBiasMinute();
				ret += StringUtil::Format("%s%02d:%02d",
					timezonBias < 0 ? "+" : "",		// %s
					(timezonBias * -1) / 60,		// %02d
					(timezonBias * -1) % 60);
				break;
			}
			case DateFormat::z: {
				Int16 timezonBias = TimeZoneBiasMinute();
				ret += StringUtil::Format("%s%d",
					timezonBias < 0 ? "+" : "",		// %s
					(timezonBias * -1) / 60);		// %d
				break;
			}
			case DateFormat::zz: {
				Int16 timezonBias = TimeZoneBiasMinute();
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


		// ���˿� ���� �ð� ���ڿ��� �򵵷� �Ѵ�.
		// @���� ���۷��� : https://www.c-sharpcorner.com/blogs/date-and-time-format-in-c-sharp-programming1
		// C# ���� ���� ������ �����ϴ�.
		
		// �߰��� ���ڿ� Ȯ���� �߻����� �ʴ´ٸ� O(n)
		JCore::String DateTime::Format(const char* fmt) const {
			JCore::String szFmt(fmt);
			JCore::String szRet(szFmt.Capacity() * 2);

			int iContinuousCount = 0;
			char cContinuousToken = '\0';

			const DateAndTime currentDateAndTime = ToDateAndTime();

			for (int i = 0; i < szFmt.Length(); i++) {
				if (FormatTokenMap_v.find(szFmt[i]) != FormatTokenMap_v.end()) {
				
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
		Int16 DateTime::TimeZoneBiasMinute() {
			TIME_ZONE_INFORMATION timeZoneInformation;
			GetTimeZoneInformation(&timeZoneInformation);
			Int16 bias = timeZoneInformation.Bias;
			return bias;
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
				Int16 uiBias = TimeZoneBiasMinute();
				epoch += (uiBias * -1) * TicksPerMinute;
			}
			return DateTime(epoch);
		}

		void DateTime::AddYear(Int32 years) {
			AddMonth(years * 12);
		}

		void DateTime::AddMonth(Int32 months) {
			DateAndTime current = ToDateAndTime();
			current.AddMonth(months);
			m_Tick = current.ToTick();
		}

		void DateTime::SubtractYear(Int32 years) {
			SubtractMonth(years * 12);
		}

		void DateTime::SubtractMonth(Int32 months) {
			DateAndTime current = ToDateAndTime();
			current.SubtractMonth(months);
			m_Tick = current.ToTick();
		}

	} // namespace Time
} // namespace JCore

