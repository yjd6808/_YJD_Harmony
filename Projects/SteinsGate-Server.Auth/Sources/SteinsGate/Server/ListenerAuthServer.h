/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:36:49 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ListenerServerCommon.h>

class ListenerAuthServer : public ListenerServerCommon
{
public:
	ListenerAuthServer(AuthServer* server, JNetwork::CommandParser* parser);
protected:
	void OnStarted() override;
	void OnConnected(JNetwork::Session* connectedSession) override;
	void OnDisconnected(JNetwork::Session* disconnetedSession) override;
	void OnSent(JNetwork::Session* sender, JNetwork::IPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* receiver, JNetwork::ICommand* recvCmd) override;
	void OnStopped() override;

	ServerType_t GetServerType() override { return ServerType::Auth; }
private:
	AuthServer* m_pAuthTcp;
};