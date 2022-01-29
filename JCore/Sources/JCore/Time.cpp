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
using namespace Detail;


// ���˿� ���� �ð� ���ڿ��� �򵵷� �Ѵ�.
String DateTime::Format(const char* fmt) const {
	String szFmt(fmt);
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
	Int64U epoch = chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch()).count();
	if (timeStandard == TimeStandard::Local) {
		Int16U uiBias = TimeZoneBiasHour();
		epoch += uiBias * TicksPerHour;
	}
	return DateTime(epoch);
}

} // namespace JCore

