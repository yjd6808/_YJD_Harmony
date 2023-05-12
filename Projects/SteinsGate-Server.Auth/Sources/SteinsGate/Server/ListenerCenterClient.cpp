/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:27 PM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "ListenerCenterClient.h"

#include <JNetwork/Packet/SendHelper.h>

#include <SteinsGate/Common/CenterCmd.h>
#include <SteinsGate/Server/R_CENTER.h>
#include <SteinsGate/Server/S_CENTER.h>


USING_NS_JC;
USING_NS_JNET;

ListenerCenterClient::ListenerCenterClient() {
	Parser.AddCommand<CmdAlreadyConnected>	(R_CENTER::RecvAlreadyConnected);
	Parser.AddCommand<CmdWhoAreYou>			(R_CENTER::RecvWhoAreYou);
	Parser.AddCommand<CmdYouNeedToDoThis>	(R_CENTER::RecvYouNeedToDoThis);
}

void ListenerCenterClient::OnConnected(SGSession* session) {
	ListenerCommonClient::OnConnected(session);

	S_CENTER::SetInformation(CoreCenterClient_v, eSendAsync);
	S_CENTER::SendItsMe(CenterClientType::Auth);
}

void ListenerCenterClient::OnDisconnected(SGSession* session) {
	ListenerCommonClient::OnDisconnected(session);
}

void ListenerCenterClient::OnSent(SGSession* session, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {
	ListenerCommonClient::OnSent(session, sentPacket, sentBytes);
}

void ListenerCenterClient::OnReceived(SGSession* session, JNetwork::ICommand* cmd) {
	ListenerCommonClient::OnReceived(session, cmd);
}

void ListenerCenterClient::OnConnectFailed(SGSession* session, Int32U errorCode) {
	ListenerCommonClient::OnConnectFailed(session, errorCode);
}