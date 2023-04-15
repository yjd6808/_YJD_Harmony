/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:28 PM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthNetMaster.h"

#include <SteinsGate/Server/AuthNetGroup.h>

USING_NS_JC;
USING_NS_JNET;

AuthNetMaster::AuthNetMaster()
	: m_bRunning(true)
{}



void AuthNetMaster::Initialize() {
	const auto spAuthNetGroup = MakeShared<AuthNetGroup>();
	AddNetGroup(1, spAuthNetGroup);
	spAuthNetGroup->Initialize();
}

void AuthNetMaster::MainLoop() {
	constexpr int PulseInterval = 100;
	Pulser pulser(PulseInterval);

	while (m_bRunning) {
		int iPulseIntervalCount = pulser.Wait();

		ProcessInputEvent();
	}
}

void AuthNetMaster::ProcessInputEvent() {

	CoreInputThread_v->PopAllEvents(m_vInputEvents);

	for (int i = 0; i < m_vInputEvents.Size(); ++i) {

		switch (m_vInputEvents[i]) {
		case AuthInputEvent::TerminateProgram:
			m_bRunning = false;
			break;
		default:
			break;
		}
	}

}
