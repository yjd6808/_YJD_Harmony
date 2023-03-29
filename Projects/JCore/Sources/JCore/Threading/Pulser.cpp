/*
 * 작성자: 윤정도
 * 생성일: 3/29/2023 6:51:07 PM
 * =====================
 *
 */

#include <JCore/Core.h>

#include <JCore/Threading/Thread.h>
#include <JCore/Threading/Pulser.h>

NS_JC_BEGIN

Pulser::Pulser(Int32U interval) : Interval(interval) {}

int Pulser::Wait()  {
	const TimeSpan gap = Watch.StopContinue();
	const Int32U uiGap = (Int32U)gap.GetTotalMicroSecondsInt();
	int iIntervalCount = 1;

	if (uiGap <= Interval) {
		Thread::Sleep(Interval - uiGap);
	} else {
		// 펄스 주기를 넘어서는 경우 배수를 맞춰 줘야하므로 갭보다 큰 인터벌 카운트를 찾는다.
		// 예를들어서 1초 인터벌인데 1.5초동안 작업을 수행해버린 경우 1.5초를 쉬게되면 타이머가 꼬이므로 2초를 쉬어줘야한다.
		iIntervalCount = uiGap / Interval + 1;
		Thread::Sleep(iIntervalCount * Interval - uiGap);
	}

	Watch.Start();
	return iIntervalCount;
}

NS_JC_END