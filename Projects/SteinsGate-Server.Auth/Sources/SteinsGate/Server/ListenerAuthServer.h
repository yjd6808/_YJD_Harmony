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
	ListenerAuthServer(AuthServer* _pServer, JNetwork::CommandParser* _pParser);

protected:
	void OnStarted() override;
	void OnConnected(JNetwork::Session* _pConnectedSession) override;
	void OnDisconnected(JNetwork::Session* _pDisconnectedSession, Int32U _errorCode) override;
	void OnSent(JNetwork::Session* _pSender, JNetwork::IPacket* _pSentPacket, Int32UL _sentBytes) override;
	void OnReceived(JNetwork::Session* _pReceiver, JNetwork::ICommand* _pRecvCmd) override;
	void OnStopped() override;

	ServerType_t GetServerType() override
	{
		return ServerType::Auth;
	}

private:
	AuthServer* authTcp_;
};
