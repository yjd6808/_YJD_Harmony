/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 2:25:07 AM
 * =====================
 *
 */


#include "NetGroup_InterServ.h"

#include "sg/_API/sgapi_Util.h"

#include "sgs/_Net/NetCore.h"
#include "sgs/_Net/NetClientListener_InterServ.h"

#include "sgs/InterServerSendHelper.h"
#include "sgs/R_SETUP_COMMON.h"
#include "sgs/R_PING_COMMON.h"
#include "sgs/R_TEST_COMMON.h"
#include "sgs/S_PING_IS_COMMON.h"

#include "sgs/CmdHost.h"
#include "sgs/CmdRelay.h"


USING_NS_JC;
USING_NS_JNET;
USING_NS_SG;

//////////////////////////////////////////////////////////////////////////////////////////
NetGroup_InterServ::NetGroup_InterServ()
: pTcp_(nullptr)
, pUdp_(nullptr)
, pParser_(dbg_new jnet::CommandParser)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
NetGroup_InterServ::~NetGroup_InterServ()
{
	JC_DELETE_SAFE(pParser_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroup_InterServ::Initialize()
{
	ServerProcessType_t processType = (ServerProcessType_t)g_cNetCore.GetProcessType();
	const ServerProcessInfo& processInfo = g_cNetCore.GetProcessInfo();
	const NetInterServerInfo& clientInfo = processInfo.GetInterServerInfo();

	CreateBufferPool({});
	CreateIOCP(clientInfo.iocpThreadCount_);
	RunIOCP();

	// --------------------------------------------------------------------
	// 파서 초기화
	// SETUP
	pParser_->AddCommand<CES_AlreadyConnected>(R_SETUP_COMMON::RECV_CES_AlreadyConnected);
	pParser_->AddCommand<CES_WhoAreYou>(R_SETUP_COMMON::RECV_CES_WhoAreYou);
	pParser_->AddCommand<CES_YouNeedToDoThis>(R_SETUP_COMMON::RECV_CES_YouNeedToDoThis);

	// PING
	pParser_->AddCommand<CES_TimeSyncAck>(R_PING_COMMON::RECV_CES_TimeSyncAck);

	// TEST
	pParser_->AddCommand<SS_P2PRelayStaticTest>(R_TEST_COMMON::RECV_SS_P2PRelayStaticTest);
	pParser_->AddCommand<SS_P2PRelayDynamicTest>(R_TEST_COMMON::RECV_SS_P2PRelayDynamicTest);
	
	// --------------------------------------------------------------------
	// 호스트 초기화
	auto pTcp = MakeShared<TcpClient>(
		pIocp_, pBufferPool_, nullptr,
		clientInfo.tcpRecvBufferSize_, clientInfo.tcpSendBufferSize_);
	pTcp->Bind(clientInfo.bindTcp_);
	pTcp->SetHandle(SH_INTER_SERV_TCP);
	AddHost(0, pTcp);

	auto pUdp = MakeShared<UdpClient>(
		pIocp_, pBufferPool_, nullptr,
		clientInfo.udpRecvBufferSize_, clientInfo.udpSendBufferSize_);
	pUdp->Bind(clientInfo.bindUdp_);
	pUdp->SetHandle(SH_INTER_SERV_UDP);
	AddHost(1, pUdp);

	pTcp_ = pTcp.Get<TcpClient*>();
	pTcp_->SetEventListener(dbg_new NetClientListener_InterServ{ processType, pParser_ });

	pUdp_ = pUdp.Get<UdpClient*>();
	pUdp_->SetEventListener(dbg_new NetClientListener_InterServ{ processType, pParser_ });
	pUdp_->RecvFromAsync();
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroup_InterServ::Finalize()
{
	NetGroup::Finalize();

	JC_DELETE_SAFE(pParser_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroup_InterServ::OnUpdate(const TimeSpan& _elapsed)
{
	SyncPeerServerTime(_elapsed);
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetGroup_InterServ::SyncPeerServerTime(const TimeSpan& _elapsed)
{
	// 피어서버는 중앙서버로 주기적으로(10초 정도마다) 시간 동기화 요청 수행
	// 따라서 중앙서버에서는 이 기능을 수행해서는 안된다.
	if (pTcp_ == nullptr || pTcp_->GetState() != Host::eConnected)
		return;

	static TimeCounter syncTimer(TimeCounterAttribute::FirstCheckFire | TimeCounterAttribute::TimeOverReset);
	syncTimer.Elapsed += _elapsed;

	if (!syncTimer.ElapsedSeconds(0.2f))
	{
		return;
	}

	S_PING_IS_COMMON::SetInformation(pTcp_, SendStrategy::SendAsync);
	S_PING_IS_COMMON::SEND_SCE_TimeSync();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool NetGroup_InterServ::ConnectCenterServer(int _tryCount)
{
	if (pTcp_ == nullptr)
	{
		_LogWarn_("인터서버 TCP 클라이언트가 초기화되어있지 않습니다.");
		return false;
	}

	if (!sgapi_IsInterServerClient())
	{
		_LogWarn_("이 프로세스는 인터서버 클라이언트가 아닙니다.");
		return false;
	}

	const NetInterServerInfo& interserverInfo = g_cNetCore.GetInterServerInfo();
	if (!interserverInfo.remoteCenterServerEp_.IsValidRemoteEndPoint())
	{
		jc_assert_msg(false, "중앙서버 원격 엔드포인트가 유효하지 않습니다.");
		return false;
	}

	constexpr int CONNECTION_TIMEOUT = 1000;
	int tryCountIndex = 0;

	_LogDebug_("중앙 서버에 접속을 시도합니다...");
	while (!pTcp_->Connect(interserverInfo.remoteCenterServerEp_, CONNECTION_TIMEOUT))
	{
		++tryCountIndex;

		if (tryCountIndex >= _tryCount)
		{
			break;
		}
	}

	if (pTcp_->GetState() == Host::eConnected)
	{
		return true;
	}

	_NetLogError_("중앙서버 접속에 실패했습니다. 프로세스를 종료합니다.");
	return false;
}
