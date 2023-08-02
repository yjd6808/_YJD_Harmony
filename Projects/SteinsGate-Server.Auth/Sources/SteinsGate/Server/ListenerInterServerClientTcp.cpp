/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:27 PM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "ListenerInterServerClientTcp.h"

#include <JNetwork/Packet/SendHelper.h>

#include <SteinsGate/Common/InterServerCmd_HOST.h>
#include <SteinsGate/Common/InterServerCmd_RELAY.h>

#include <SteinsGate/Server/R_INTERSERVER.h>
#include <SteinsGate/Server/S_INTERSERVER.h>

USING_NS_JC;
USING_NS_JNET;

ListenerInterServerClientTcp::ListenerInterServerClientTcp() {
	Parser.AddCommand<CES_AlreadyConnected>		(R_INTERSERVER::RecvAlreadyConnected);
	Parser.AddCommand<CES_WhoAreYou>			(R_INTERSERVER::RecvWhoAreYou);
	Parser.AddCommand<CES_YouNeedToDoThis>		(R_INTERSERVER::RecvYouNeedToDoThis);
	Parser.AddCommand<CES_TimeSyncAck>			(R_INTERSERVER::RecvTimeSyncAck);
	Parser.AddCommand<CmdP2PRelayStaticTest>	(R_INTERSERVER::RecvP2PRelayStaticTest);
	Parser.AddCommand<CmdP2PRelayDynamicTest>	(R_INTERSERVER::RecvP2PRelayDynamicTest);
}

void ListenerInterServerClientTcp::OnConnected(SGSession* session) {
	ListenerCommonClient::OnConnected(session);

	S_INTERSERVER::SetInformation(CoreInterServerClientTcp_v, eSendAsync, SingleServerType::Center);
	S_INTERSERVER::SendItsMe(InterServerClientType::Auth, InterServerSendHelperBase::GetSenderId());
}

void ListenerInterServerClientTcp::OnDisconnected(SGSession* session) {
	ListenerCommonClient::OnDisconnected(session);
}

void ListenerInterServerClientTcp::OnSent(SGSession* session, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {
	ListenerCommonClient::OnSent(session, sentPacket, sentBytes);
}

void ListenerInterServerClientTcp::OnReceived(SGSession* session, JNetwork::ICommand* cmd) {
	OnReceivedInterServerCmd(session, cmd);
}

void ListenerInterServerClientTcp::OnConnectFailed(SGSession* session, Int32U errorCode) {
	ListenerCommonClient::OnConnectFailed(session, errorCode);
}