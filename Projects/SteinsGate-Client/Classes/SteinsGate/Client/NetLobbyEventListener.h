/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:36:49 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/NetClientEventListener.h>

class NetLobbyEventListener : public NetClientEventListener
{
public:
	void OnConnected(JNetwork::Session* session) override;
	void OnDisconnected(JNetwork::Session* session) override;
	void OnConnectFailed(JNetwork::Session* session, Int32U errorCode) override;
	void OnSent(JNetwork::Session* session, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* session, JNetwork::ICommand* cmd) override;
};