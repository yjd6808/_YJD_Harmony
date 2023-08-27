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

#include <SteinsGate/Common/R_SETUP_COMMON.h>
#include <SteinsGate/Common/R_PING_COMMON.h>
#include <SteinsGate/Common/R_TEST_COMMON.h>

#include <SteinsGate/Common/CmdHost.h>
#include <SteinsGate/Common/CmdRelay.h>

#include "S_PING_IS_COMMON.h"

USING_NS_JC;
USING_NS_JNET;

InterServerClientNetGroup::InterServerClientNetGroup()
	: m_pInterServerClientTcp(nullptr)
	, m_pInterServerClientUdp(nullptr)
	, m_pParser(dbg_new SGCommandParser)
{}

InterServerClientNetGroup::~InterServerClientNetGroup() {
	JCORE_DELETE_SAFE(m_pParser);
}

void InterServerClientNetGroup::Initialize() {
	if (Core::ServerProcessInfo == nullptr) {
		_LogWarn_("서버 정보가 없어서 네트그룹 초기화 실패 [인터서버 네트그룹]");
		return;
	}

	InitializeBufferPool();
	InitializeIOCP();
	InitializeParser();
	InitializeInterServerTcp();
	InitializeInterServerUdp();
	
	InterServerSendHelperBase::InitSingleServerIds();
	InterServerSendHelperBase::InitSingleServerDestinations();
}

void InterServerClientNetGroup::Finalize() {
	NetGroup::Finalize();

	JCORE_DELETE_SAFE(m_pParser);
}

void InterServerClientNetGroup::ProcessUpdate(const TimeSpan& elpased) {
	SyncPeerServerTime(elpased);
	OnUpdate(elpased);
}


void InterServerClientNetGroup::InitializeParser() {
	// SETUP
	m_pParser->AddCommand<CES_AlreadyConnected>		(R_SETUP_COMMON::RECV_CES_AlreadyConnected);
	m_pParser->AddCommand<CES_WhoAreYou>			(R_SETUP_COMMON::RECV_CES_WhoAreYou);
	m_pParser->AddCommand<CES_YouNeedToDoThis>		(R_SETUP_COMMON::RECV_CES_YouNeedToDoThis);

	// PING
	m_pParser->AddCommand<CES_TimeSyncAck>			(R_PING_COMMON::RECV_CES_TimeSyncAck);

	// TEST
	m_pParser->AddCommand<SS_P2PRelayStaticTest>	(R_TEST_COMMON::RECV_SS_P2PRelayStaticTest);
	m_pParser->AddCommand<SS_P2PRelayDynamicTest>	(R_TEST_COMMON::RECV_SS_P2PRelayDynamicTest);
}

void InterServerClientNetGroup::SyncPeerServerTime(const TimeSpan& elapsed) {
	// 피어서버는 중앙서버로 주기적으로(10초 정도마다) 시간 동기화 요청 수행
	// 따라서 중앙서버에서는 이 기능을 수행해서는 안된다.
	if (!IsPeerServer())
		return;

	if (m_pInterServerClientTcp == nullptr || m_pInterServerClientTcp->GetState() != Host::eConnected)
		return;

	static TimeCounter s_Timer(TimeCounterAttribute::FirstCheckFire | TimeCounterAttribute::TimeOverReset);
	s_Timer.Elapsed += elapsed;

	if (!s_Timer.ElapsedSeconds(0.2f)) {
		return;
	}

	S_PING_IS_COMMON::SetInformation(m_pInterServerClientTcp, SendStrategy::SendAsync);
	S_PING_IS_COMMON::SEND_SCE_TimeSync();

	// auto pPacket = dbg_new SinglePacket<SCE_TimeSync>;
	// pPacket->Cmd.PeerServerTime.Tick = DateTime::Now().Tick;
	// m_pInterServerClientTcp->SendAsync(pPacket);
}

bool InterServerClientNetGroup::ConnectCenterServer(int tryCount) {

	if (m_pInterServerClientTcp == nullptr) {
		_LogWarn_("인터서버 TCP 클라이언트가 초기화되어있지 않습니다.");
		return false;
	}

	constexpr int CenterConnectionTimeout = 1000;

	int iTryCount = 0;

	_LogDebug_("중앙 서버에 접속을 시도합니다...");
	while (!m_pInterServerClientTcp->Connect(Core::ServerProcessInfoPackage->Center.RemoteEP, CenterConnectionTimeout)) {
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

