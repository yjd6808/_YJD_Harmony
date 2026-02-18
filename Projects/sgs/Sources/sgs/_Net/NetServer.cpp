/*
 * 작성자: 윤정도
 * 생성일: 4/21/2023 9:02:24 PM
 * =====================
 *
 */


#include "Core.h"
#include "NetServer.h"
#include "NetSession.h"

#include <sgs/S_SETUP_IS_COMMON.h>

#include "NetGroup_InterServ.h"

USING_NS_JC;
USING_NS_JNET;
USING_NS_SG;

////////////////////////////////////////////////////////////////////////////////////////////////////
NetServer::NetServer(const IOCPPtr& _pIocp, const MemoryPoolAbstractPtr& _pBufferAllocator)
: TcpServer(_pIocp, _pBufferAllocator)
, bootState_(ServerBootState::Stopped)
, serverInfo_(NetServerInfo::dummy_)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
jnet::TcpSession* NetServer::CreateSession()
{
	return dbg_new NetSession(this, pIocp_, pBufferAllocator_, serverInfo_.tcpRecvBufferSize_, serverInfo_.tcpSendBufferSize_);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool NetServer::Start()
{
	if (serverInfo_.serverType_ == ServerType::None)
	{
		_LogError_("서버 타입이 설정되지 않았습니다.");
		return false;
	}

	if (!serverInfo_.bindTcp_.IsValidEndPoint())
	{
		_LogError_("바인드 엔드포인트가 유효하지 않습니다. (%s)", serverInfo_.bindTcp_.ToString().Source());
		return false;
	}

	return jnet::TcpServer::Start(serverInfo_.bindTcp_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void NetServer::OnStarted()
{
	bootState_ = ServerBootState::Launched;
	if (serverInfo_.serverType_ == ServerType::Center)
		return;

	S_SETUP_IS_COMMON::SetInformation(g_cNetGroup_InterServ.GetTcp(), SendStrategy::SendAsync);
	S_SETUP_IS_COMMON::SEND_SCE_NotifyBootState(serverId_, (ServerType_t)serverInfo_.serverType_, ServerBootState::Launched);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void NetServer::OnStartFailed(Int32U _errorCode)
{
	bootState_ = ServerBootState::Stopped;
	// 중앙서버는 자신이 부트상태를 관리하므로
	if (serverInfo_.serverType_ == ServerType::Center)
		return;
	S_SETUP_IS_COMMON::SetInformation(g_cNetGroup_InterServ.GetTcp(), SendStrategy::SendAsync);
	S_SETUP_IS_COMMON::SEND_SCE_NotifyOrderFailed(serverId_, (ServerType_t)serverInfo_.serverType_, CenterOrder::LaunchServer, _errorCode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void NetServer::OnStopped()
{
	// 중앙서버는 자신이 부트상태를 관리하므로
	bootState_ = ServerBootState::Stopped;
	if (serverInfo_.serverType_ == ServerType::Center)
		return;

	S_SETUP_IS_COMMON::SetInformation(g_cNetGroup_InterServ.GetTcp(), SendStrategy::SendAsync);
	S_SETUP_IS_COMMON::SEND_SCE_NotifyBootState(serverId_, (ServerType_t)serverInfo_.serverType_, ServerBootState::Stopped);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void NetServer::OnUpdate(const jc::TimeSpan& _elapsed)
{
	(void)_elapsed;
}
