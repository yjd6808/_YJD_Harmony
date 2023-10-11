/*
 * 작성자: 윤정도
 * 생성일: 10/10/2023 5:06:40 PM
 * =====================
 *
 */


#include "Pch.h"
#include "L_GAME.h"

void L_GAME::OnConnected(JNetwork::Session* session) {
}

void L_GAME::OnConnectFailed(JNetwork::Session* session, Int32U errorCode) {
}

void L_GAME::OnDisconnected(JNetwork::Session* session) {
}

void L_GAME::OnSent(JNetwork::Session* session, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) {
}

void L_GAME::OnReceived(JNetwork::Session* session, JNetwork::ICommand* recvCmd) {
}

void L_GAME::OnReceived(JNetwork::Session* session, JNetwork::IRecvPacket* recvPacket) {
}
