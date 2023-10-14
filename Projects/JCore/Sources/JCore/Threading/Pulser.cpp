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

Pulser::Pulser(Int32U intervalMiliseconds, Mode mode /* = eSliceCycle */, JCORE_IN PulserStatistics* statistics /* = nullptr */)
	: Interval(intervalMiliseconds)
	, Statistics(statistics) {
	switch (mode) {
	case eSliceCycle: m_pWaitor = dbg_new SliceWaitor(*this); break;
	case eExactCycle: m_pWaitor = dbg_new ExactWaitor(*this); break;
	}

}

Pulser::~Pulser() {
	JCORE_DELETE_SAFE(m_pWaitor);
}

void Pulser::Start() {
	Watch.Start();
}

TimeSpan Pulser::Wait()  {
	return m_pWaitor->Wait();
}

TimeSpan Pulser::SliceWaitor::Wait() {
	TimeSpan workTime = m_Pulser.Watch.GetElapsed();

	for (;;) {
		const auto uiWorkTimeMs = (Int32U)workTime.GetTotalMiliSecondsInt();
		if (uiWorkTimeMs >= m_Pulser.Interval) {
			break;
		}

		Thread::Sleep(1);
		workTime = m_Pulser.Watch.GetElapsed();
	}

	if (m_Pulser.Statistics) {
		m_Pulser.Statistics->PulseCount += 1;
		m_Pulser.Statistics->SleepIntervalLast = (Int32U)workTime.GetTotalMiliSecondsInt();
		m_Pulser.Statistics->TotalSleepTime += m_Pulser.Statistics->SleepIntervalLast;
	}

	m_Pulser.Watch.Start();
	return workTime;
}

TimeSpan Pulser::ExactWaitor::Wait() {
	TimeSpan workTime = m_Pulser.Watch.GetElapsed();
	Int32U uiWorkTime = (Int32U)workTime.GetTotalMiliSecondsInt();

	if (uiWorkTime <= m_Pulser.Interval) {
		Thread::Sleep(m_Pulser.Interval - uiWorkTime);
	} else {
		// 펄스 주기를 넘어서는 경우 배수를 맞춰 줘야하므로 갭보다 큰 인터벌 카운트를 찾는다.
		// 예를들어서 1초 인터벌인데 1.5초동안 작업을 수행해버린 경우 1.5초를 쉬게되면 타이머가 꼬이므로 2초를 쉬어줘야한다.
		const int iIntervalCount = uiWorkTime / m_Pulser.Interval + 1;
		Thread::Sleep(iIntervalCount * m_Pulser.Interval - uiWorkTime);
	}
	workTime = m_Pulser.Watch.GetElapsed();

	if (m_Pulser.Statistics) {
		m_Pulser.Statistics->PulseCount += 1;
		m_Pulser.Statistics->SleepIntervalLast = (Int32U)workTime.GetTotalMiliSecondsInt();
		m_Pulser.Statistics->TotalSleepTime += m_Pulser.Statistics->SleepIntervalLast;
	}

	m_Pulser.Watch.Start();
	return workTime;
}

NS_JC_END
