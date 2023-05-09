/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:36:49 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/EventListener/ServerEventListener.h>

class ListenerAuthServer : public JNetwork::ServerEventListener
{
protected:
	void OnStarted() override;
	void OnConnected(JNetwork::Session* connectedSession) override;
	void OnDisconnected(JNetwork::Session* disconnetedSession) override;
	void OnSent(JNetwork::Session* sender, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* receiver, JNetwork::ICommand* cmd) override;
	void OnStopped() override;
public:
	ListenerAuthServer();

	JNetwork::CommandParser Parser;
};