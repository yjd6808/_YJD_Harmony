/*
 * 작성자: 윤정도
 * 생성일: 6/13/2023 5:34:53 AM
 * =====================
 *
 */


#include <JCoreTest/CoreTest.h>
#include <JCore/Threading/Scheduler.h>

#if TEST_SchedulerTest == ON

DateTime start;
AtomicInt counter;

static void JoinTest(Scheduler::JoinStrategy strategy) {
	int i = 0;
	for (;;)
	{
		start = DateTime::Now();
		counter = 0;

		Console::WriteLine(" ======================= [%d] ======================= ", ++i);
		Scheduler s(6);

		s.IntervalAt([](SchedulerTask* task) {
			Console::WriteLine("0.1초당 한번 (%.1f초 경화 후 호출됨)", DateTime::Now().Diff(start).GetTotalSeconds());
			++counter;
		}, DateTime::Now() + TimeSpan::FromMiliSeocnd(R(2000, 2001)), TimeSpan::FromMiliSeocnd(R(100, 101)), 5);

		s.IntervalAt([](SchedulerTask* task) {
			Console::WriteLine("0.2초당 한번 (%.1f초 경화 후 호출됨)", DateTime::Now().Diff(start).GetTotalSeconds());
			++counter;
		}, DateTime::Now() + TimeSpan::FromMiliSeocnd(R(1000, 1001)), TimeSpan::FromMiliSeocnd(R(200, 201)), 5);

		s.IntervalAt([](SchedulerTask* task) {
			Console::WriteLine("0.3초당 한번 (%.1f초 경화 후 호출됨)", DateTime::Now().Diff(start).GetTotalSeconds());
			++counter;
		}, DateTime::Now(), TimeSpan::FromMiliSeocnd(R(300, 301)), 5);

		Thread::Sleep(1);
		s.Join(strategy);

		if (strategy == Scheduler::JoinStrategy::WaitOnlyRunningTask) {
			EXPECT_TRUE(counter == 1);	// 0.3초마다 한번씩 수행되는 태스크 하나만 딱 한번 수행될 것이기 때문
		} else if (strategy == Scheduler::JoinStrategy::WaitAllTasks) {
			EXPECT_TRUE(counter == 15);	// 모든 태스크가가 5번씩 정상 실행
		}

		if (i == 3) {
			break;
		}
	}
}

TEST(SchedulerTest, WaitOnlyRunningTask) {
	LeakCheck;
	JoinTest(Scheduler::JoinStrategy::WaitOnlyRunningTask);
}

TEST(SchedulerTest, WaitAll) {
	LeakCheck;
	JoinTest(Scheduler::JoinStrategy::WaitAllTasks);
}

#endif // TEST_SchedulerTest == ON