/*
 * 작성자: 윤정도
 * 생성일: 4/21/2023 9:02:24 PM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "CommonServer.h"

#include <SteinsGate/Common/S_SETUP_IS_COMMON.h>

USING_NS_JC;
USING_NS_JNET;

////////////////////////////////////////////////////////////////////////////////////////////////////
CommonServer::CommonServer(const IOCPPtr& _pIocp, const MemoryPoolAbstractPtr& _pBufferAllocator)
: TcpServer(_pIocp, _pBufferAllocator)
, bootState_(ServerBootState::Stopped)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CommonServer::OnStarted()
{
	const ServerType_t serverType = GetServerType();

	// 중앙서버는 자신이 부트상태를 관리하므로
	if (serverType == ServerType::Center)
		return;

	bootState_ = ServerBootState::Launched;
	S_SETUP_IS_COMMON::SetInformation(Core::InterServerClientTcp, SendStrategy::SendAsync);
	S_SETUP_IS_COMMON::SEND_SCE_NotifyBootState(Core::ServerProcessInfo->serverId_, serverType,
	                                            ServerBootState::Launched);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CommonServer::OnStartFailed(Int32U _errorCode)
{
	const ServerType_t serverType = GetServerType();

	// 중앙서버는 자신이 부트상태를 관리하므로
	if (serverType == ServerType::Center)
		return;

	bootState_ = ServerBootState::Stopped;
	S_SETUP_IS_COMMON::SetInformation(Core::InterServerClientTcp, SendStrategy::SendAsync);
	S_SETUP_IS_COMMON::SEND_SCE_NotifyOrderFailed(Core::ServerProcessInfo->serverId_, serverType,
	                                              CenterOrder::LaunchServer, _errorCode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CommonServer::OnStopped()
{
	const ServerType_t serverType = GetServerType();

	// 중앙서버는 자신이 부트상태를 관리하므로
	if (serverType == ServerType::Center)
		return;

	bootState_ = ServerBootState::Stopped;
	S_SETUP_IS_COMMON::SetInformation(Core::InterServerClientTcp, SendStrategy::SendAsync);
	S_SETUP_IS_COMMON::SEND_SCE_NotifyBootState(Core::ServerProcessInfo->serverId_, serverType,
	                                            ServerBootState::Stopped);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CommonServer::OnUpdate(const JCore::TimeSpan& _elapsed)
{
	(void)_elapsed;
}
