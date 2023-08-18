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

CommonNetMaster::CommonNetMaster(int updatePerSecond)
	: m_iUpdatePerSecond(updatePerSecond)
	, m_bRunning(true)
{}

void CommonNetMaster::Initialize() {

}


void CommonNetMaster::ProcessMainUpdate() {
	PulserStatistics pulseStat;
	Pulser pulser(1000 / m_iUpdatePerSecond, Pulser::eSliceCycle, &pulseStat);
	TimeSpan elapsed;

	pulser.Start();
	while (m_bRunning) {

		if (CoreCLIThread_v)
			CoreCLIThread_v->ProcessInputs();

		ProcessSubUpdate(elapsed);
		OnUpdate(elapsed);

		elapsed = pulser.Wait();
		// Console::WriteLine("%d", elapsed.GetTotalMiliSecondsInt());
	}

	Finalize();
	OnStopped();
}

void CommonNetMaster::ProcessSubUpdate(const TimeSpan& elapsed) {
	if (CoreCommonNetGroup_v)
		CoreCommonNetGroup_v->ProcessUpdate(elapsed);

	if (CoreInterServerClientNetGroup_v)
		CoreInterServerClientNetGroup_v->ProcessUpdate(elapsed);
}

