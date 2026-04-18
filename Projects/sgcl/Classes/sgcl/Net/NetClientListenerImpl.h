/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:36:49 PM
 * =====================
 *
 */


#pragma once

#include "sg/Net/NetClientListener.h"
#include "sg/Net/NetListenerHelper.h"

class NetClientListenerImpl : public sg::NetClientListener
{
public:
	NetClientListenerImpl(ServerType_t _connectedServerType);

protected:
	void OnConnected(jnet::Session* _pSession) override;
	void OnDisconnected(jnet::Session* _pSession, _u32 _errorCode) override;
	void OnConnectFailed(jnet::Session* _pSession, _u32 _errorCode) override;
	void OnSent(jnet::Session* _pSession, jnet::IPacket* _pSendPacket, _u32l _sentBytes) override;
	void OnReceivedCmd(jnet::Session* _pSession, jnet::ICommand* _pRecvCmd) override;
	void OnReceivedPacket(jnet::Session* _pSession, jnet::RecvedPacket* _pRecvPacket) override;

private:
	ServerType_t connectedServerType_;
};
