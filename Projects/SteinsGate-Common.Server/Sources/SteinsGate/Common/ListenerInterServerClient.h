/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:13 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ListenerClientCommon.h>

class ListenerInterServerClient : public ListenerClientCommon
{
public:
	ListenerInterServerClient(ServerProcessType_t _serverProcessType, JNetwork::CommandParser* _pParser);

protected:
	void OnConnected(JNetwork::Session* _pSession) override;
	void OnDisconnected(JNetwork::Session* _pSession, Int32U _errorCode) override;
	void OnSent(JNetwork::Session* _pSession, JNetwork::IPacket* _pSentPacket, Int32UL _sentBytes) override;
	void OnReceived(JNetwork::Session* _pSession, JNetwork::ICommand* _pRecvCmd) override;
	void OnReceived(JNetwork::Session* _pSession, JNetwork::RecvedCommandPacket* _pRecvPacket) override;
	void OnConnectFailed(JNetwork::Session* _pSession, Int32U _errorCode) override;

private:
	ServerProcessType_t serverProcessType_;
};
