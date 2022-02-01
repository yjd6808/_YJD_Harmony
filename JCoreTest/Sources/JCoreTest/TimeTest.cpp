/*
	작성자 : 윤정도
	시간 관련 기능 테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/String.h>
#include <JCore/Comparator.h>
#include <JCore/Time.h>

using namespace JCore;
using namespace JCore::Time;
using namespace std;

#define Print ON

#if TEST_TimeTest == ON

/*=====================================================================================
									DateTime
 =====================================================================================*/

// 2022년 2월 1일 16시 11분 33초 715 716
constexpr DateTime g_TestTime(63779328693715716);

// 2022년 1월 31일 0시 0분 0초 0.0
constexpr DateTime g_TestCleanTime(63779184000000000);

void PrintDate(DateTime& dt) {
#if Print == ON
	cout << dt.Format("yyyy-MM-dd(dddd) tt hh:mm:ss.ffffff") << "\n";
#endif
}

void ResetDate(DateTime& dt) {
	dt = g_TestTime;
}

void ResetCleanDate(DateTime& dt) {
	dt = g_TestCleanTime;
}

TEST(DateTimeTest, Get) {
	EXPECT_TRUE(g_TestTime.GetAMPM() == AMPM::PM);
	EXPECT_TRUE(g_TestTime.GetDayOfWeek() == DayOfWeek::Tuesday);
	EXPECT_TRUE(g_TestTime.GetDayOfYear() == 32);
	EXPECT_TRUE(g_TestTime.GetYear() == 2022);
	EXPECT_TRUE(g_TestTime.GetMonth() == 2);
	EXPECT_TRUE(g_TestTime.GetDay() == 1);
	EXPECT_TRUE(g_TestTime.GetHour() == 16);
	EXPECT_TRUE(g_TestTime.GetMinute() == 11);
	EXPECT_TRUE(g_TestTime.GetSecond() == 33);
	EXPECT_TRUE(g_TestTime.GetMiliSecond() == 715);
	EXPECT_TRUE(g_TestTime.GetMicroSecond() == 716);
}

// 년, 월 연산 테스트
TEST(DateTimeTest, Operation_Year_Month) {
	DateTime dt = g_TestTime;

	EXPECT_NO_THROW(dt.AddYear(7976)); // 9998
	ResetDate(dt);
	EXPECT_NO_THROW(dt.AddYear(7977)); // 9999
	ResetDate(dt);
	EXPECT_THROW(dt.AddYear(7978), std::overflow_error); // 10000
	EXPECT_THROW(dt.AddYear(7978), std::overflow_error); // 10001

	EXPECT_NO_THROW(dt.SubtractYear(2020)); ResetDate(dt);	// 2
	EXPECT_NO_THROW(dt.SubtractYear(2021)); ResetDate(dt);	// 1
	EXPECT_THROW(dt.SubtractYear(2022), std::underflow_error);	// 0
	EXPECT_THROW(dt.SubtractYear(2023), std::underflow_error);	// 0

	
	EXPECT_TRUE(dt.AddMonth(1).ToDate() == Date(2022, 3, 1)); ResetDate(dt);
	EXPECT_TRUE(dt.AddMonth(2).ToDate() == Date(2022, 4, 1)); ResetDate(dt);
	EXPECT_TRUE(dt.AddMonth(3).ToDate() == Date(2022, 5, 1)); ResetDate(dt);
	EXPECT_TRUE(dt.AddMonth(4).ToDate() == Date(2022, 6, 1)); ResetDate(dt);

	EXPECT_TRUE(dt.SubtractMonth(1).ToDate() == Date(2022, 1, 1)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractMonth(2).ToDate() == Date(2021, 12, 1)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractMonth(3).ToDate() == Date(2021, 11, 1)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractMonth(4).ToDate() == Date(2021, 10, 1)); ResetDate(dt);
}

// 년, 월 연산
TEST(DateTimeTest, Operation_Day_Hour_Minute_Second_Mili_Micro) {
	DateTime dt = g_TestTime;

	EXPECT_TRUE(dt.SubtractDay(1).ToDate() == Date(2022, 1, 31)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractDay(-1).ToDate() == Date(2022, 2, 2)); ResetDate(dt);
	EXPECT_TRUE(dt.AddDay(1).ToDate() == Date(2022, 2, 2)); ResetDate(dt);
	EXPECT_TRUE(dt.AddDay(-1).ToDate() == Date(2022, 1, 31)); ResetDate(dt);

	EXPECT_TRUE(dt.SubtractHour(24).ToDate() == Date(2022, 1, 31)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractHour(-24).ToDate() == Date(2022, 2, 2)); ResetDate(dt);
	EXPECT_TRUE(dt.AddHour(24).ToDate() == Date(2022, 2, 2)); ResetDate(dt);
	EXPECT_TRUE(dt.AddHour(-24).ToDate() == Date(2022, 1, 31)); ResetDate(dt);

	EXPECT_TRUE(dt.SubtractHour(17).ToDate() < Date(2022, 2, 1)); ResetDate(dt);  // 17시간빼면 1월 31일 됨
	EXPECT_TRUE(dt.SubtractHour(16).ToDate() == Date(2022, 2, 1)); ResetDate(dt); // 16시간빼면 2월 1일 00시 xx

	EXPECT_TRUE(dt.ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 715, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractMinute(30).ToDateAndTime() == DateAndTime(2022, 2, 1, 15, 41, 33, 715, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractMinute(-90).ToDateAndTime() == DateAndTime(2022, 2, 1, 17, 41, 33, 715, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.AddMinute(90).ToDateAndTime() == DateAndTime(2022, 2, 1, 17, 41, 33, 715, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.AddMinute(-30).ToDateAndTime() == DateAndTime(2022, 2, 1, 15, 41, 33, 715, 716)); ResetDate(dt);

	EXPECT_TRUE(dt.SubtractSecond(30).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 03, 715, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractSecond(33).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 00, 715, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractSecond(34).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 10, 59, 715, 716)); ResetDate(dt);

	EXPECT_TRUE(dt.SubtractSecond(-30).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 12, 03, 715, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractSecond(-28).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 12, 01, 715, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractSecond(-27).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 12, 00, 715, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractSecond(-26).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 59, 715, 716)); ResetDate(dt);

	EXPECT_TRUE(dt.AddSecond(-30).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 03, 715, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.AddSecond(-33).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 00, 715, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.AddSecond(-34).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 10, 59, 715, 716)); ResetDate(dt);

	EXPECT_TRUE(dt.AddSecond(30).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 12, 03, 715, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.AddSecond(28).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 12, 01, 715, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.AddSecond(27).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 12, 00, 715, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.AddSecond(26).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 59, 715, 716)); ResetDate(dt);

	// DateAndTime(2022, 2, 1, 16, 11, 33, 715, 716)
	EXPECT_TRUE(dt.SubtractMiliSecond(714).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 1, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractMiliSecond(715).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 0, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractMiliSecond(716).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 32, 999, 716)); ResetDate(dt);

	EXPECT_TRUE(dt.AddMiliSecond(-714).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 1, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.AddMiliSecond(-715).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 0, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.AddMiliSecond(-716).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 32, 999, 716)); ResetDate(dt);

	EXPECT_TRUE(dt.SubtractMiliSecond(1715).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 32, 0, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractMiliSecond(1716).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 31, 999, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractMiliSecond(1717).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 31, 998, 716)); ResetDate(dt);

	EXPECT_TRUE(dt.AddMiliSecond(284).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 999, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.AddMiliSecond(285).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 34, 0, 716)); ResetDate(dt);
	EXPECT_TRUE(dt.AddMiliSecond(286).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 34, 1, 716)); ResetDate(dt);
	
	// DateAndTime(2022, 2, 1, 16, 11, 33, 715, 716)
	EXPECT_TRUE(dt.SubtractMicroSecond(716).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 715, 0)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractMicroSecond(717).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 714, 999)); ResetDate(dt);

	EXPECT_TRUE(dt.AddMicroSecond(284).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 716, 0)); ResetDate(dt);
	EXPECT_TRUE(dt.AddMicroSecond(283).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 715, 999)); ResetDate(dt);
	EXPECT_TRUE(dt.AddMicroSecond(285).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 716, 1)); ResetDate(dt);

	EXPECT_TRUE(dt.AddMicroSecond(-716).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 715, 0)); ResetDate(dt);
	EXPECT_TRUE(dt.AddMicroSecond(-717).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 714, 999)); ResetDate(dt);

	EXPECT_TRUE(dt.SubtractMicroSecond(-284).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 716, 0)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractMicroSecond(-283).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 715, 999)); ResetDate(dt);
	EXPECT_TRUE(dt.SubtractMicroSecond(-285).ToDateAndTime() == DateAndTime(2022, 2, 1, 16, 11, 33, 716, 1)); ResetDate(dt);

	ResetCleanDate(dt);

	EXPECT_TRUE(dt.SubtractMicroSecond(1).ToDateAndTime() == DateAndTime(2022, 1, 30, 23, 59, 59, 999, 999)); ResetCleanDate(dt);
} 


// DateTime, DateAndTime, TimeSpan과 연산
TEST(DateTimeTest, Operation_DateTime_DateAndTime_TimeSpan) {
	DateTime dt = g_TestCleanTime;

	EXPECT_TRUE(dt >= DateAndTime(2022, 1, 30, 23, 59, 59, 999, 999));
	EXPECT_TRUE(dt >= g_TestCleanTime);
	EXPECT_TRUE(dt == g_TestCleanTime);
	EXPECT_TRUE(dt <= g_TestCleanTime);
	EXPECT_TRUE(dt < DateAndTime(2022, 1, 31, 0, 0, 0, 0, 1));
	EXPECT_TRUE(dt <= DateAndTime(2022, 1, 31, 0, 0, 0, 0, 1));

	EXPECT_TRUE(dt >= DateAndTime(2022, 1, 30, 23, 59, 59, 999, 999).ToDateTime());
	EXPECT_TRUE(dt >= DateAndTime(2022, 1, 31, 0, 0, 0, 0, 0).ToDateTime());
	EXPECT_TRUE(dt > DateAndTime(2022, 1, 30, 23, 59, 59, 999, 999).ToDateTime());
	EXPECT_TRUE(dt == DateAndTime(2022, 1, 31, 0, 0, 0, 0, 0).ToDateTime());
	EXPECT_TRUE(dt < DateAndTime(2022, 1, 31, 0, 0, 0, 0, 1).ToDateTime());
	EXPECT_TRUE(dt <= DateAndTime(2022, 1, 31, 0, 0, 0, 0, 0).ToDateTime());
	EXPECT_TRUE(dt <= DateAndTime(2022, 1, 31, 0, 0, 0, 0, 1).ToDateTime());

	EXPECT_TRUE(dt >= TimeSpan(DateAndTime(2022, 1, 30, 23, 59, 59, 999, 999).ToTick()));
	EXPECT_TRUE(dt >= TimeSpan(DateAndTime(2022, 1, 31, 0, 0, 0, 0, 0).ToTick()));
	EXPECT_TRUE(dt > TimeSpan(DateAndTime(2022, 1, 30, 23, 59, 59, 999, 999).ToTick()));
	EXPECT_TRUE(dt == TimeSpan(DateAndTime(2022, 1, 31, 0, 0, 0, 0, 0).ToTick()));
	EXPECT_TRUE(dt < TimeSpan(DateAndTime(2022, 1, 31, 0, 0, 0, 0, 1).ToTick()));
	EXPECT_TRUE(dt <= TimeSpan(DateAndTime(2022, 1, 31, 0, 0, 0, 0, 0).ToTick()));
	EXPECT_TRUE(dt <= TimeSpan(DateAndTime(2022, 1, 31, 0, 0, 0, 0, 1).ToTick()));
}



#endif // TEST_TimeTest == ON




