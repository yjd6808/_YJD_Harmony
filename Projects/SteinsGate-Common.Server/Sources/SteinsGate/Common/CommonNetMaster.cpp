/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:28 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "CommonNetMaster.h"

#include <SteinsGate/Common/CommonNetGroup.h>
#include <SteinsGate/Common/InterServerClientNetGroup.h>
#include <SteinsGate/Common/InterServerSendHelper.h>

USING_NS_JC;
USING_NS_JNET;

CommonNetMaster::CommonNetMaster()
	: m_iPulseInterval(100)
	, m_bRunning(true)
{}

void CommonNetMaster::Initialize() {

}


void CommonNetMaster::ProcessMainLoop() {
	PulserStatistics pulseStat;
	Pulser pulser(m_iPulseInterval, &pulseStat);

	while (m_bRunning) {
		pulser.Wait();

		ProcessInputEvent();
		ProcessSubLoop(&pulseStat);
		OnLoop(&pulseStat);
	}

	OnStopped();
}

void CommonNetMaster::ProcessSubLoop(PulserStatistics* pulseStat) {
	if (CoreCommonNetGroup_v)
		CoreCommonNetGroup_v->ProcessLoop(pulseStat);

	if (CoreInterServerClientNetGroup_v)
		CoreInterServerClientNetGroup_v->ProcessLoop(pulseStat);
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

