/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:27 PM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "ListenerCenterClient.h"

#include <SteinsGate/Common/CenterCmd.h>
#include <SteinsGate/Server/RecvCenter.h>

ListenerCenterClient::ListenerCenterClient() {
}

void ListenerCenterClient::OnConnected() {
	_LogInfo_("중앙 서버에 성공적으로 접속하였습니다.");
}

void ListenerCenterClient::OnDisconnected() {
	_LogInfo_("중앙 서버와 연결이 끊어졌습니다.");
}

void ListenerCenterClient::OnSent(JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {

}

void ListenerCenterClient::OnReceived(JNetwork::ICommand* cmd) {

}

void ListenerCenterClient::OnConnectFailed(Int32U errorCode) {
	_LogDebug_("중앙 서버접속에 실패했습니다. (%u)", errorCode);
}