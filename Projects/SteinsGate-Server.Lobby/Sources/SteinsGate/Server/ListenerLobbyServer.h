/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ListenerCommonServer.h>

class ListenerLobbyServer : public ListenerCommonServer
{
public:
	ListenerLobbyServer();
protected:
	void OnStarted() override;
	void OnConnected(JNetwork::Session* connectedSession) override;
	void OnDisconnected(JNetwork::Session* disconnetedSession) override;
	void OnSent(JNetwork::Session* sender, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* receiver, JNetwork::ICommand* cmd) override;
	void OnStopped() override;

	ServerType_t GetServerType() override { return ServerType::Lobby; }
};