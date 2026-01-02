/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:13 PM
 * =====================
 *
 */


#pragma once

#include <sgs/ListenerClientCommon.h>

class ListenerInterServerClient : public ListenerClientCommon
{
public:
	ListenerInterServerClient(ServerProcessType_t _serverProcessType, jnet::CommandParser* _pParser);

protected:
	void OnConnected(jnet::Session* _pSession) override;
	void OnDisconnected(jnet::Session* _pSession, Int32U _errorCode) override;
	void OnSent(jnet::Session* _pSession, jnet::IPacket* _pSentPacket, Int32UL _sentBytes) override;
	void OnReceived(jnet::Session* _pSession, jnet::ICommand* _pRecvCmd) override;
	void OnReceived(jnet::Session* _pSession, jnet::RecvedCommandPacket* _pRecvPacket) override;
	void OnConnectFailed(jnet::Session* _pSession, Int32U _errorCode) override;

private:
	ServerProcessType_t serverProcessType_;
};
