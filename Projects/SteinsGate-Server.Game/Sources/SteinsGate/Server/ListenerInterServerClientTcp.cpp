/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Game.h"
#include "GameCoreHeader.h"
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
	S_INTERSERVER::SendItsMe(InterServerClientType::Game, InterServerSendHelperBase::GetSenderId());
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