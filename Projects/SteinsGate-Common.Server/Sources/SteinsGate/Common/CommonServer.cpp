/*
 * 작성자: 윤정도
 * 생성일: 4/21/2023 9:02:24 PM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "CommonServer.h"

#include <SteinsGate/Common/S_INTERSERVER_COMMON.h>

USING_NS_JC;
USING_NS_JNET;

CommonServer::CommonServer(const IOCPPtr& iocp, const MemoryPoolAbstractPtr& bufferAllocator)
	: TcpServer(iocp, bufferAllocator)
	, m_eBootState(ServerBootState::Stopped)
{}

void CommonServer::OnStarted() {
	const ServerType_t eType = GetServerType();

	// 중앙서버는 자신이 부트상태를 관리하므로
	if (eType == ServerType::Center)
		return;

	m_eBootState = ServerBootState::Launched;
	S_INTERSERVER_COMMON::SetInformation(CoreInterServerClientTcp_v, eSendAsync);
	S_INTERSERVER_COMMON::SEND_SCE_NotifyBootState(CoreServerProcessInfo_v->ServerId, eType, ServerBootState::Launched);
}

void CommonServer::OnStartFailed(Int32U errorCode) {
	const ServerType_t eType = GetServerType();

	// 중앙서버는 자신이 부트상태를 관리하므로
	if (eType == ServerType::Center)
		return;

	S_INTERSERVER_COMMON::SetInformation(CoreInterServerClientTcp_v, eSendAsync);
	S_INTERSERVER_COMMON::SEND_SCE_NotifyOrderFailed(CoreServerProcessInfo_v->ServerId, eType, CenterOrder::LaunchServer, errorCode);
}

void CommonServer::OnStopped() {
	const ServerType_t eType = GetServerType();

	// 중앙서버는 자신이 부트상태를 관리하므로
	if (eType == ServerType::Center)
		return;

	m_eBootState = ServerBootState::Stopped;
	S_INTERSERVER_COMMON::SetInformation(CoreInterServerClientTcp_v, eSendAsync);
	S_INTERSERVER_COMMON::SEND_SCE_NotifyBootState(CoreServerProcessInfo_v->ServerId, eType, ServerBootState::Stopped);
}

void CommonServer::OnUpdate(const JCore::TimeSpan& elapsed) {

}
