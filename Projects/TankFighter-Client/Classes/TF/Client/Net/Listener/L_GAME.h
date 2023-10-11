/*
 * 작성자: 윤정도
 * 생성일: 10/10/2023 5:04:38 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/EventListener/ClientEventListener.h>

class L_GAME : JNetwork::ClientEventListener
{
	void OnConnected(JNetwork::Session* session) override;
	void OnConnectFailed(JNetwork::Session* session, Int32U errorCode) override;
	void OnDisconnected(JNetwork::Session* session) override;
	void OnSent(JNetwork::Session* session, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* session, JNetwork::ICommand* recvCmd) override;
	void OnReceived(JNetwork::Session* session, JNetwork::IRecvPacket* recvPacket) override;
};