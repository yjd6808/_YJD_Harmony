#include "header.h"

int R(int min, int max) { return Random::GenerateInt(min, max); }

USING_NS_STD_CHRONO;

int main() {
	Console::SetSize(800, 600);
	Random::EngineInitialize();

	int i = 0;
	for (;;)
	{
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

		//Thread::Sleep(1);
		s.Join(Scheduler::JoinStrategy::WaitOnlyRunningTask);

		if (i == 100)
			break;
	}
	return 0;
}

