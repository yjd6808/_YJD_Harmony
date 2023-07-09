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
#include <SteinsGate/Common/InterServerCmd_HOST.h>

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

void InterServerClientNetGroup::ProcessLoop(PulserStatistics* pulserStat) {
	OnLoopCommon(pulserStat);
	OnLoop(pulserStat);
}

void InterServerClientNetGroup::OnLoopCommon(PulserStatistics* pulserStat) {
	SyncPeerServerTime(pulserStat);
}

void InterServerClientNetGroup::SyncPeerServerTime(PulserStatistics* pulserStat) {
	// 피어서버는 중앙서버로 주기적으로(10초 정도마다) 시간 동기화 요청 수행
	// 따라서 중앙서버에서는 이 기능을 수행해서는 안된다.
	if (!IsPeerServer())
		return;

	if (m_pInterServerClientTcp == nullptr || m_pInterServerClientTcp->GetState() != Host::eConnected)
		return;

	static PulserTimer s_Timer{ 0, PulserTimer::eCheckReset };
	s_Timer.ElapsedMs += pulserStat->SleepIntervalLast;

	if (!s_Timer.ElapsedSeconds(10)) {
		return;
	}

	auto pPacket = dbg_new SinglePacket<CmdTimeSync>;
	pPacket->Cmd.PeerServerTime.Tick = DateTime::Now().Tick;
	m_pInterServerClientTcp->SendAsync(pPacket);
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

