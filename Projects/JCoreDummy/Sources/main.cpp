#include <header.h>
#include <queue>

#include <timeapi.h>

using Watch = StopWatch<StopWatchMode::HighResolution>;

struct Pulse
{
	Pulse(Int64U interval) : Interval(interval) {}

	
	void Wait() {
		Int64U uiGap = Watch.StopContinue().GetTotalMiliSecondsInt();

		if (uiGap <= Interval) {
			Thread::Sleep(Interval - uiGap);
		} else {
			// 펄스 주기를 넘어서는 경우 배수를 맞춰 줘야하므로 갭보다 큰 인터벌 카운트를 찾는다.
			// 예를들어서 1초 인터벌인데 1.5초동안 작업을 수행해버린 경우 1.5초를 쉬게되면 타이머가 꼬이므로 2초를 쉬어줘야한다.
			Int64U uiIntervalCount = (uiGap / Interval + 1);
			Thread::Sleep(uiIntervalCount * Interval - uiGap);
		}

		Watch.Start();
	}

	Watch Watch;
	Int64U Interval;
};

int main() {
	Pulse pulse(1000);

	for (;;) {
		pulse.Wait();

		printf("안녕하세요\n");
	}
}
