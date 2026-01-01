/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:36:49 PM
 * =====================
 *
 */


#pragma once

#include <sgs/ListenerServerCommon.h>

class ListenerAuthServer : public ListenerServerCommon
{
public:
	ListenerAuthServer(AuthServer* _pServer, jnet::CommandParser* _pParser);

protected:
	void OnStarted() override;
	void OnConnected(jnet::Session* _pConnectedSession) override;
	void OnDisconnected(jnet::Session* _pDisconnectedSession, Int32U _errorCode) override;
	void OnSent(jnet::Session* _pSender, jnet::IPacket* _pSentPacket, Int32UL _sentBytes) override;
	void OnReceived(jnet::Session* _pReceiver, jnet::ICommand* _pRecvCmd) override;
	void OnStopped() override;

	ServerType_t GetServerType() override
	{
		return ServerType::Auth;
	}

private:
	AuthServer* authTcp_;
};
