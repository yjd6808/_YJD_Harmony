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

//////////////////////////////////////////////////////////////////////////////////////////
InterServerClientNetGroup::InterServerClientNetGroup()
: interServerClientTcp_(nullptr)
, interServerClientUdp_(nullptr)
, parser_(dbg_new SGCommandParser)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
InterServerClientNetGroup::~InterServerClientNetGroup()
{
	JCORE_DELETE_SAFE(parser_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void InterServerClientNetGroup::Initialize()
{
	if (Core::ServerProcessInfo == nullptr)
	{
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

//////////////////////////////////////////////////////////////////////////////////////////
void InterServerClientNetGroup::Finalize()
{
	NetGroup::Finalize();

	JCORE_DELETE_SAFE(parser_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void InterServerClientNetGroup::ProcessUpdate(const TimeSpan& _elapsed)
{
	SyncPeerServerTime(_elapsed);
	OnUpdate(_elapsed);
}

//////////////////////////////////////////////////////////////////////////////////////////
void InterServerClientNetGroup::InitializeParser()
{
	// SETUP
	parser_->AddCommand<CES_AlreadyConnected>(R_SETUP_COMMON::RECV_CES_AlreadyConnected);
	parser_->AddCommand<CES_WhoAreYou>(R_SETUP_COMMON::RECV_CES_WhoAreYou);
	parser_->AddCommand<CES_YouNeedToDoThis>(R_SETUP_COMMON::RECV_CES_YouNeedToDoThis);

	// PING
	parser_->AddCommand<CES_TimeSyncAck>(R_PING_COMMON::RECV_CES_TimeSyncAck);

	// TEST
	parser_->AddCommand<SS_P2PRelayStaticTest>(R_TEST_COMMON::RECV_SS_P2PRelayStaticTest);
	parser_->AddCommand<SS_P2PRelayDynamicTest>(R_TEST_COMMON::RECV_SS_P2PRelayDynamicTest);
}

//////////////////////////////////////////////////////////////////////////////////////////
void InterServerClientNetGroup::SyncPeerServerTime(const TimeSpan& _elapsed)
{
	// 피어서버는 중앙서버로 주기적으로(10초 정도마다) 시간 동기화 요청 수행
	// 따라서 중앙서버에서는 이 기능을 수행해서는 안된다.
	if (!IsPeerServer())
		return;

	if (interServerClientTcp_ == nullptr || interServerClientTcp_->GetState() != Host::eConnected)
		return;

	static TimeCounter syncTimer(TimeCounterAttribute::FirstCheckFire | TimeCounterAttribute::TimeOverReset);
	syncTimer.Elapsed += _elapsed;

	if (!syncTimer.ElapsedSeconds(0.2f))
	{
		return;
	}

	S_PING_IS_COMMON::SetInformation(interServerClientTcp_, SendStrategy::SendAsync);
	S_PING_IS_COMMON::SEND_SCE_TimeSync();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool InterServerClientNetGroup::ConnectCenterServer(int _tryCount)
{
	if (interServerClientTcp_ == nullptr)
	{
		_LogWarn_("인터서버 TCP 클라이언트가 초기화되어있지 않습니다.");
		return false;
	}

	constexpr int CENTER_CONNECTION_TIMEOUT = 1000;

	int tryCountIndex = 0;

	_LogDebug_("중앙 서버에 접속을 시도합니다...");
	while (!interServerClientTcp_->Connect(Core::ServerProcessInfoPackage->center_.remoteEp_,
	                                       CENTER_CONNECTION_TIMEOUT))
	{
		++tryCountIndex;

		if (tryCountIndex >= _tryCount)
		{
			break;
		}
	}

	if (interServerClientTcp_->GetState() == Host::eConnected)
	{
		return true;
	}

	_NetLogError_("중앙서버 접속에 실패했습니다. 프로세스를 종료합니다.");
	return false;
}
