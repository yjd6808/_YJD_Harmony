/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:13 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/EventListener/ClientEventListener.h>
#include <JNetwork/Packet/CommandParser.h>

class ListenerCenterClient : public JNetwork::ClientEventListener
{
public:
	ListenerCenterClient();
protected:
	void OnConnected() override;
	void OnDisconnected() override;
	void OnSent(JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::ICommand* cmd) override;
	void OnConnectFailed(Int32U errorCode) override;
private:
	JNetwork::CommandParser m_Parser;
};

