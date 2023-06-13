#include "header.h"

int R(int min, int max) { return Random::GenerateInt(min, max); }

USING_NS_STD_CHRONO;
AtomicInt counter;
DateTime dt;

int main() {
	Console::SetSize(800, 600);
	Random::EngineInitialize();

	int i = 0;
	
	
	for (;;)
	{
		dt = DateTime::Now();
		counter = 0;

		Console::WriteLine(" ======================= [%d] ======================= ", ++i);
		Scheduler s(6);

		s.IntervalAt([](SchedulerTask* task) {
			Console::WriteLine("1초당 한번 (%.1f초 경화 후 호출됨)", DateTime::Now().Diff(dt).GetTotalSeconds());
			++counter;
		}, DateTime::Now() + TimeSpan::FromMiliSeocnd(R(1000, 1001)), TimeSpan::FromMiliSeocnd(R(1000, 1001)), 5);

		s.IntervalAt([](SchedulerTask* task) {
			Console::WriteLine("2초당 한번 (%.1f초 경화 후 호출됨)", DateTime::Now().Diff(dt).GetTotalSeconds());
			++counter;
		}, DateTime::Now() + TimeSpan::FromMiliSeocnd(R(0, 150)), TimeSpan::FromMiliSeocnd(R(2000, 2001)), 5);

		s.IntervalAt([](SchedulerTask* task) {
			Console::WriteLine("3초당 한번 (%.1f초 경화 후 호출됨)", DateTime::Now().Diff(dt).GetTotalSeconds());
			++counter;
		}, DateTime::Now(), TimeSpan::FromMiliSeocnd(R(3000, 3001)), 5);

		Thread::Sleep(1);
		s.Join(Scheduler::JoinStrategy::WaitOnlyRunningTask);
	}
}

