/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 12:08:19 PM
 * =====================
 *
 */


#pragma once


#include <JNetwork/EventListener/ServerEventListener.h>

class L_GAME : public JNetwork::ServerEventListener
{
	 void OnStarted() override;
	 void OnStartFailed(Int32U errorCode) override;
	 void OnStopped() override;
	 void OnConnected(JNetwork::Session* session) override;
	 void OnConnectFailed(JNetwork::Session* session, Int32U errorCode) override;
	 void OnDisconnected(JNetwork::Session* session, Int32U errorCode) override;
	 void OnSent(JNetwork::Session* session, JNetwork::IPacket* sentPacket, Int32UL sentBytes) override;
	 void OnReceived(JNetwork::Session* session, JNetwork::ICommand* recvCmd) override;
	 void OnReceived(JNetwork::Session* session, JNetwork::RecvedCommandPacket* recvPacket) override;
};
