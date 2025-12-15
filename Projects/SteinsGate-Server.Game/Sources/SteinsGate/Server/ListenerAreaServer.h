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
	ListenerAreaServer(AreaServer* _pServer, JNetwork::CommandParser* _pParser);
protected:
	void OnStarted() override;
	void OnConnected(JNetwork::Session* _pConnectedSession) override;
	void OnDisconnected(JNetwork::Session* _pDisconnectedSession, Int32U _errorCode) override;
	void OnSent(JNetwork::Session* _pSender, JNetwork::IPacket* _pSentPacket, Int32UL _sentBytes) override;
	void OnReceived(JNetwork::Session* _pReceiver, JNetwork::ICommand* _pRecvCmd) override;
	void OnReceived(JNetwork::Session* _pSession, JNetwork::RecvedCommandPacket* _pRecvPacket) override;
	void OnStopped() override;

	ServerType_t GetServerType() override { return ServerType::Area; }
private:
	AreaServer* areaTcp_;
};
