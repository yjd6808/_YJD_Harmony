#include "header.h"

using namespace std::chrono_literals;

USING_NS_STD_CHRONO;

bool ready = false;

#define STD_
#ifdef STD_
condition_variable condvar;
mutex lk;
#else
ConditionVariable condvar;
NormalLock lk;
#endif

int main() {
	Thread th{ [](void*) {
		for (;;) {
			#ifdef STD_
			unique_lock lg(lk);
			bool r = condvar.wait_for(lg, seconds{2}, [] { return ready; });
			if (!r) {
				Console::WriteLine("타임아웃");
			} else {
				Console::WriteLine("신호수신");
				ready = false;
			}
			#else
			NormalLockGuard lock_guard(lk);
			int r = condvar.WaitFor(lock_guard, TimeSpan::FromMiliSeocnd(2000), [] { return ready; });
			if (r == CvStatus::eTimeout) {
				Console::WriteLine("타임아웃");
			} else {
				Console::WriteLine("신호수신");
				ready = false;
			}
			#endif



		}
	}};


	while (true) {
		int a;
		std::cin >> a;

		if (a == 1) {
#ifdef STD_
			unique_lock lg(lk);
			ready = true;
			condvar.notify_all();
#else
			NormalLockGuard lock_guard(lk);
			ready = true;
			condvar.NotifyAll();
#endif
			break;
		}
	}


	th.Join();


	Random::EngineInitialize();
	return 0;
}







