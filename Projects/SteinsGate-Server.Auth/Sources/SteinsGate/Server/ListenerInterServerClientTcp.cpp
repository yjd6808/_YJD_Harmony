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
	Parser.AddCommand<CmdAlreadyConnected>		(R_INTERSERVER::RecvAlreadyConnected);
	Parser.AddCommand<CmdWhoAreYou>				(R_INTERSERVER::RecvWhoAreYou);
	Parser.AddCommand<CmdYouNeedToDoThis>		(R_INTERSERVER::RecvYouNeedToDoThis);
	Parser.AddCommand<CmdTimeSyncAck>			(R_INTERSERVER::RecvTimeSyncAck);
	Parser.AddCommand<CmdP2PRelayStaticTest>	(R_INTERSERVER::RecvP2PRelayStaticTest);
	Parser.AddCommand<CmdP2PRelayDynamicTest>	(R_INTERSERVER::RecvP2PRelayDynamicTest);
}

void ListenerInterServerClientTcp::OnConnected(SGSession* session) {
	ListenerCommonClient::OnConnected(session);

	S_INTERSERVER::SetInformation(CoreInterServerClientTcp_v, eSendAsync, SingleServerType::Center);
	S_INTERSERVER::SendItsMe(InterServerClientType::Auth, InterServerSendHelperBase::GetSenderId());


	// TODO: UDP 송신 테스트 (추후 지울 것)
	S_INTERSERVER::SetInformation(CoreInterServerClientUdp_v, eSendToAsync, SingleServerType::Center);
	S_INTERSERVER::SendItsMe(InterServerClientType::Auth, InterServerSendHelperBase::GetSenderId());

	// TODO: 인터서버 통신 테스트 테스트 (추후 지울 것)
	S_INTERSERVER::SetInformation(CoreInterServerClientTcp_v, eSendAsync, SingleServerType::Auth);
	S_INTERSERVER::SendP2PRelayStaticTest("안녕1");
	S_INTERSERVER::SendP2PRelayDynamicTest("안녕2");
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