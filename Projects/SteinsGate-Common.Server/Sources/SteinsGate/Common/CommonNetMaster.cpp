/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:28 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "CommonNetMaster.h"

#include "CommonNetGroup.h"

USING_NS_JC;
USING_NS_JNET;

CommonNetMaster::CommonNetMaster()
	: m_iPulseInterval(100)
	, m_bRunning(true)
{}


void CommonNetMaster::ProcessMainLoop() {
	Pulser pulser(m_iPulseInterval);

	while (m_bRunning) {
		const int iPulseIntervalCount = pulser.Wait();
		const int iSleepMs = m_iPulseInterval * iPulseIntervalCount;

		ProcessInputEvent();
		ProcessSubLoop(iSleepMs);
		OnLoop(iSleepMs);
	}

	OnStopped();
}

void CommonNetMaster::ProcessSubLoop(int sleepMs) {
	CoreCommonNetGroup_v->ProcessLoop(sleepMs);
}

void CommonNetMaster::ProcessInputEvent() {
	CoreInputThread_v->PopAllEvents(m_vInputEvents);

	for (int i = 0; i < m_vInputEvents.Size(); ++i) {
		OnCapturedInputEvent(m_vInputEvents[i]);
	}
}

void CommonNetMaster::OnCapturedInputEvent(int inputEvent) {
	switch (inputEvent) {
	case AuthInputEvent::TerminateProgram:
		Terminate();
		break;
	default:
		break;
	}
}

