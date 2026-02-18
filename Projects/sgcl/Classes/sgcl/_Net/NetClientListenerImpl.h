/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:36:49 PM
 * =====================
 *
 */


#pragma once

#include <sg/_Net/NetClientListener.h>
#include <sg/_Net/NetListenerHelper.h>

class NetClientListenerImpl : public sg::NetClientListener
{
public:
	NetClientListenerImpl(ClientConnectServerType_t _connectedServerType);

protected:
	void OnConnected(jnet::Session* _pSession) override;
	void OnDisconnected(jnet::Session* _pSession, Int32U _errorCode) override;
	void OnConnectFailed(jnet::Session* _pSession, Int32U _errorCode) override;
	void OnSent(jnet::Session* _pSession, jnet::IPacket* _pSendPacket, Int32UL _sentBytes) override;
	void OnReceived(jnet::Session* _pSession, jnet::ICommand* _pRecvCmd) override;
	void OnReceived(jnet::Session* _pSession, jnet::RecvedCommandPacket* _pRecvPacket) override;

private:
	ClientConnectServerType_t connectedServerType_;
};
