/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:36:49 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>

class SGNetClientEventListener : public JNetwork::TcpServerEventListener
{
public:
	SGNetClientEventListener(SGCommandParser* parser);

	void OnStarted() override;
	void OnConnected(JNetwork::Session* connectedSession) override;
	void OnDisconnected(JNetwork::Session* disconnetedSession) override;
	void OnSent(JNetwork::Session* sender, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* receiver, JNetwork::ICommand* cmd) override;
	void OnStopped() override;
protected:
	SGCommandParser* m_pParser;
};