/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:27 PM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "ListenerInterServerClient.h"

#include <JNetwork/Packet/SendHelper.h>

#include <SteinsGate/Common/CmdHost.h>
#include <SteinsGate/Common/CmdRelay.h>

#include <SteinsGate/Common/S_SETUP_IS_COMMON.h>
#include <SteinsGate/Common/R_SETUP_COMMON.h>

USING_NS_JC;
USING_NS_JNET;

ListenerInterServerClient::ListenerInterServerClient(ServerProcessType_t serverProcessType, SGCommandParser* parser)
	: ListenerClientCommon(parser)
	, m_eServerProcessType(serverProcessType)
{}

void ListenerInterServerClient::OnConnected(SGSession* session) {
	ListenerClientCommon::OnConnected(session);

	S_SETUP_IS_COMMON::SetInformation(Core::InterServerClientTcp, SendStrategy::SendAsync, SingleServerType::Center);
	S_SETUP_IS_COMMON::SEND_SCE_ItsMe(m_eServerProcessType, Core::ServerProcessInfo->ServerId);
}

void ListenerInterServerClient::OnDisconnected(SGSession* session) {
	ListenerClientCommon::OnDisconnected(session);
}

void ListenerInterServerClient::OnSent(SGSession* session, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {
	ListenerClientCommon::OnSent(session, sentPacket, sentBytes);
}

void ListenerInterServerClient::OnReceived(SGSession* session, JNetwork::ICommand* cmd) {
	ListenerClientCommon::OnReceived(session, cmd);
}

void ListenerInterServerClient::OnReceived(SGSession* session, JNetwork::IRecvPacket* recvPacket) {
	ListenerClientCommon::OnReceived(session, recvPacket);
}

void ListenerInterServerClient::OnConnectFailed(SGSession* session, Int32U errorCode) {
	ListenerClientCommon::OnConnectFailed(session, errorCode);
}
