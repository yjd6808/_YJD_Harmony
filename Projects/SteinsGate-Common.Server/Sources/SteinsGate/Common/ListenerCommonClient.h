/*
 * 작성자: 윤정도
 * 생성일: 5/12/2023 9:36:48 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/EventListener/ClientEventListener.h>
#include <JNetwork/Packet/CommandParser.h>

class ListenerCommonClient : public JNetwork::ClientEventListener
{
protected:
	void OnConnected(JNetwork::Session* session) override;
	void OnConnectFailed(JNetwork::Session* session, Int32U errorCode) override;
	void OnDisconnected(JNetwork::Session* session) override;
	void OnSent(JNetwork::Session* sessionm, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* session, JNetwork::ICommand* cmd) override;
public:
	JNetwork::CommandParser Parser;
};
