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
	Parser.AddCommand(CmdAlreadyConnected_SC,	R_CENTER::RecvAlreadyConnected);
	Parser.AddCommand(CmdWhoAreYou_SC,			R_CENTER::RecvWhoAreYou);
	Parser.AddCommand(CmdYouNeedToDoThis_SC,	R_CENTER::RecvYouNeedToDoThis);
		
}

void ListenerCenterClient::OnConnected() {
	_LogInfo_("중앙 서버에 성공적으로 접속하였습니다.");
	S_CENTER::SetInformation(CoreCenterClient_v, eSendAsync);
	S_CENTER::SendItsMe(CenterClientType::Auth);
}

void ListenerCenterClient::OnDisconnected() {
	_LogInfo_("중앙 서버와 연결이 끊어졌습니다.");
}

void ListenerCenterClient::OnSent(JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {

}

void ListenerCenterClient::OnReceived(JNetwork::ICommand* cmd) {
	if (!Parser.RunCommand(CoreCenterClient_v, cmd)) {
		_LogWarn_("커맨드: %d 수행 실패 (Center)", cmd->GetCommand());
	}

	if (SendHelper::SendInformation.Strategy == eSendAlloc)
		SendHelper::FlushSendBuffer();
}

void ListenerCenterClient::OnConnectFailed(Int32U errorCode) {
	_LogDebug_("중앙 서버접속에 실패했습니다. (%u)", errorCode);
}