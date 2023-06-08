#include "header.h"
#include <future>

int main() {
	Console::SetSize(800, 600);
	Random::EngineInitialize();
	int r = 0;
	for (;;) {
		Console::WriteLine("========================== %d =====================", ++r);
		ThreadPool pool(12);

		Task<int> t1 = pool.Run([](int a, int b) {
			Console::WriteLine("태스크1");
			Thread::Sleep(Random::GenerateInt(100, 4000));
			return 100;
		}, 100, 200);
		t1.SetDebugName("태스크1");

		Task<void> t2 = pool.Run([](int a, int b) {
			Console::WriteLine("태스크2");
			Thread::Sleep(Random::GenerateInt(100, 4000));
		}, 100, 200);
		t2.SetDebugName("태스크2");

		Task<void> taskArr[20];

		for (int i = 3; i <= 8; ++i) {
			taskArr[i - 3] = pool.Run([=](int a, int b) {
				Console::WriteLine("태스크%d", i);
				Thread::Sleep(Random::GenerateInt(100, 4000));
			}, 100, 200);
			taskArr[i - 3].SetDebugName(StringUtil::Format("태스크%d", i));
		}

		Thread th1{ [=](void*) {
			int r;
			if (!t1.Wait(&r)) {
				Console::WriteLine("태스크1 작업 취소됨");
			} else {
				Console::WriteLine("태스크1 작업 완료 %d", r);
			}
			JCORE_PASS;
		}};

		Thread th2{ [=](void*) {
			if (!t2.Wait()) {
				Console::WriteLine("태스크2 작업 취소됨");
			} else {
				Console::WriteLine("태스크2 작업 작업완료");
			}
		}};

		Thread th3{ [=](void*) {
			for (int i = 0; i <= 5; ++i) {
				TaskWaitResult r;
				taskArr[i].Wait(&r);
				Console::WriteLine("태스크%d 작업완료(대기 결과 %d)", i+ 3, r);
			}
		} };


		Thread::Sleep(Random::GenerateInt(10, 2000));
		Console::WriteLine("조인1");
		pool.Join();
		Console::WriteLine("조인2");
		th1.Join();
		Console::WriteLine("조인3");
		th2.Join();
		Console::WriteLine("조인4");
		Console::WriteLine("조인4-State:%d", (int)taskArr[0].GetContextState());
		Console::WriteLine("조인4-State:%d", (int)taskArr[1].GetContextState());
		Console::WriteLine("조인4-State:%d", (int)taskArr[2].GetContextState());
		Console::WriteLine("조인4-State:%d", (int)taskArr[3].GetContextState());
		Console::WriteLine("조인4-State:%d", (int)taskArr[4].GetContextState());
		Console::WriteLine("조인4-State:%d", (int)taskArr[5].GetContextState());
		th3.Join();
		Console::WriteLine("조인5");
		if (r == 1)  break;
	}

	return 0;
}

