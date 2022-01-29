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
using namespace Detail;


// 포맷에 따라 시간 문자열을 얻도록 한다.
String DateTime::Format(const char* fmt) const {
	String szFmt(fmt);
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
	Int64U epoch = chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch()).count();
	if (timeStandard == TimeStandard::Local) {
		Int16U uiBias = TimeZoneBiasHour();
		epoch += uiBias * TicksPerHour;
	}
	return DateTime(epoch);
}

} // namespace JCore

