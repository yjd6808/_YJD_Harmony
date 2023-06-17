/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 2:25:07 AM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "InterServerClientNetGroup.h"

#include <SteinsGate/Common/InterServerSendHelper.h>

USING_NS_JC;
USING_NS_JNET;

InterServerClientNetGroup::InterServerClientNetGroup()
	: m_pInterServerClientTcp(nullptr)
	, m_pInterServerClientUdp(nullptr)
{}

void InterServerClientNetGroup::Initialize() {
	InitializeBufferPool();
	InitializeIOCP();
	InitializeInterServerTcp();
	InitializeInterServerUdp();

	InterServerSendHelperBase::InitDefaultToId(CoreServerProcessInfoPackage_v->Center.ServerId);
	InterServerSendHelperBase::InitSingleServerIds();
	InterServerSendHelperBase::InitSingleServerDestinations();
}

void InterServerClientNetGroup::ProcessLoop(PulserStatistics* pulseStat) {
	OnLoop(pulseStat);
}

bool InterServerClientNetGroup::ConnectCenterServer(int tryCount) {
	DebugAssertMsg(m_pInterServerClientTcp != nullptr, "인터서버 TCP 클라이언트가 초기화되어있지 않습니다.");

	constexpr int CenterConnectionTimeout = 1000;

	int iTryCount = 0;

	_LogDebug_("중앙 서버에 접속을 시도합니다...");
	while (!m_pInterServerClientTcp->Connect(CoreServerProcessInfoPackage_v->Center.RemoteEP, CenterConnectionTimeout)) {
		++iTryCount;

		if (iTryCount >= tryCount) {
			break;
		}
	}

	if (m_pInterServerClientTcp->GetState() == Host::eConnected) {
		return true;
	}

	_NetLogError_("중앙서버 접속에 실패했습니다. 프로세스를 종료합니다.");
	return false;
}

