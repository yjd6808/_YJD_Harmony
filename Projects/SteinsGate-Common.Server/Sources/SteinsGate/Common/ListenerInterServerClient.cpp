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

#include <SteinsGate/Common/InterServerCmd_HOST.h>
#include <SteinsGate/Common/InterServerCmd_RELAY.h>

#include <SteinsGate/Common/S_INTERSERVER_COMMON.h>
#include <SteinsGate/Common/R_INTERSERVER_COMMON.h>

USING_NS_JC;
USING_NS_JNET;

ListenerInterServerClient::ListenerInterServerClient(InterServerClientType_t interServerClientType, SGCommandParser* parser)
	: ListenerClientCommon(parser)
	, m_eInterServerClientType(interServerClientType)
{}

void ListenerInterServerClient::OnConnected(SGSession* session) {
	ListenerClientCommon::OnConnected(session);

	S_INTERSERVER_COMMON::SetInformation(CoreInterServerClientTcp_v, eSendAsync, SingleServerType::Center);
	S_INTERSERVER_COMMON::SendItsMe(m_eInterServerClientType, InterServerSendHelperBase::GetSenderId());
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
