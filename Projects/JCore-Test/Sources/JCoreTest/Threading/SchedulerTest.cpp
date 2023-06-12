/*
 * 작성자: 윤정도
 * 생성일: 6/13/2023 5:34:53 AM
 * =====================
 *
 */


#include <JCoreTest/CoreTest.h>
#include <JCore/Threading/Scheduler.h>

#if TEST_SchedulerTest == ON

TEST(SchedulerTest, General) {
	LeakCheck;

	int i = 0;
	DateTime begin;
	DateTime end;
	for (;;)
	{
		begin = DateTime::Now();
		Console::WriteLine(" ======================= [%d] ======================= ", ++i);
		Scheduler s(6);

		s.IntervalAt([](SchedulerTask* task) {
			Thread::Sleep(R(10, 100));
			Console::WriteLine("A");
		}, DateTime::Now() + TimeSpan::FromMiliSeocnd(R(0, 300)), TimeSpan::FromMiliSeocnd(R(50, 400)), 5);

		s.IntervalAt([](SchedulerTask* task) {
			Thread::Sleep(R(1, 100));
			Console::WriteLine("B");
		}, DateTime::Now() + TimeSpan::FromMiliSeocnd(R(0, 150)), TimeSpan::FromMiliSeocnd(R(10, 50)), 5);

		s.IntervalAt([](SchedulerTask* task) {
			Thread::Sleep(R(1, 50));
			Console::WriteLine("C");
		}, DateTime::Now(), TimeSpan::FromMiliSeocnd(R(0, 300)), 5);

		s.Join(Scheduler::JoinStrategy::WaitOnlyRunningTask);

		end = DateTime::Now();
		EXPECT_TRUE(end.Diff(begin).GetTotalMiliSecondsInt() <= 300);

		if (i == 3)
			break;
	}
}

#endif // TEST_SchedulerTest == ON