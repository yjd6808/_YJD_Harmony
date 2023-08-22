/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ListenerServerCommon.h>
#include <SteinsGate/Server/AreaServer.h>

class ListenerAreaServer : public ListenerServerCommon
{
public:
	ListenerAreaServer(AreaServer* server, JNetwork::CommandParser* parser);
protected:
	void OnStarted() override;
	void OnConnected(JNetwork::Session* connectedSession) override;
	void OnDisconnected(JNetwork::Session* disconnetedSession) override;
	void OnSent(JNetwork::Session* sender, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* receiver, JNetwork::ICommand* recvCmd) override;
	void OnReceived(JNetwork::Session* session, JNetwork::IRecvPacket* recvPacket) override;
	void OnStopped() override;

	ServerType_t GetServerType() override { return ServerType::Area; }
private:
	AreaServer* m_pAreaTcp;
};