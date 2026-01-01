/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */

#pragma once

#include <sgs/ListenerServerCommon.h>
#include <sgs_game/ChatServer.h>

class ListenerChatServer : public ListenerServerCommon
{
public:
	ListenerChatServer(ChatServer* _pServer, jnet::CommandParser* _pParser);
protected:
	void OnStarted() override;
	void OnConnected(jnet::Session* _pConnectedSession) override;
	void OnDisconnected(jnet::Session* _pDisconnetedSession, Int32U _errorCode) override;
	void OnSent(jnet::Session* _pSender, jnet::IPacket* _pSentPacket, Int32UL _sentBytes) override;
	void OnReceived(jnet::Session* _pReceiver, jnet::ICommand* _pRecvCmd) override;
	void OnReceived(jnet::Session* _pSession, jnet::RecvedCommandPacket* _pRecvPacket) override;
	void OnStopped() override;

	ServerType_t GetServerType() override
	{
		return ServerType::Chat;
	}
private:
	ChatServer* chatTcp_;
};
