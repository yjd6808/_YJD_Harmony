/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JNetwork/Host/TcpServerEventListener.h>
#include <TF/Parser/CommandParser.h>

class GameServerEventListener : public JNetwork::TcpServerEventListener
{
public:
	GameServerEventListener(CommandParser* parser) : m_pParser(parser) {}
protected:
	void OnStarted() override;
	void OnConnected(JNetwork::TcpSession* connectedSession) override;
	void OnDisconnected(JNetwork::TcpSession* disconnetedSession) override;
	void OnSent(JNetwork::TcpSession* sender, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::TcpSession* receiver, JNetwork::ICommand* cmd) override;
	void OnPaused() override;
	void OnResume() override;
	void OnStopped() override;

	CommandParser* m_pParser;
};



