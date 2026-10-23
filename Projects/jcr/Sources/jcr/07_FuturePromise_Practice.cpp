#include "Core.h"

#include <future>

//////////////////////////////////////////////////////////////////////////////////////////
int call_07_FuturePromise(int _argc, char** _argv)
{
	auto futureTask =
		std::async([]() {
		for (int i = 0; i < 5; ++i) {
			// Simulate some work
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			_tprintf(_T("Async task running: %d\n"), i);
		}
	});

	std::jthread backgroundThread([&futureTask]() {
		futureTask.wait();
	});

	Console::WriteLine(_T("Hello World"));

	return 0;
}
