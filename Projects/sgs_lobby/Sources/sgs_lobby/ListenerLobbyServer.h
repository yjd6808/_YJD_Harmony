/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */

#pragma once

#include <sgs/_Net/NetServerListener.h>
#include <sgs_lobby/LobbyServer.h>

class ListenerLobbyServer : public NetServerListener
{
public:
	ListenerLobbyServer(LobbyServer* _pServer, jnet::CommandParser* _pParser);
protected:
	void OnStarted() override;
	void OnConnected(jnet::Session* _pConnectedSession) override;
	void OnDisconnected(jnet::Session* _pDisconnectedSession, _u32 _errorCode) override;
	void OnSent(jnet::Session* _pSender, jnet::IPacket* _pSentPacket, _u32l _sentBytes) override;
	void OnReceived(jnet::Session* _pReceiver, jnet::ICommand* _pRecvCmd) override;
	void OnReceived(jnet::Session* _pSession, jnet::RecvedCmdPacket* _pRecvPacket) override;
	void OnStopped() override;

	ServerType_t GetServerType() override
	{
		return ServerType::Lobby;
	}
private:
	LobbyServer* lobbyTcp_;
};
