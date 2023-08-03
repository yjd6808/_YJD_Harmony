/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ListenerInterServerCommonClient.h>

class ListenerInterServerClientTcp : public ListenerInterServerCommonClient
{
public:
	ListenerInterServerClientTcp();
protected:
	void OnConnected(SGSession* session) override;
	void OnDisconnected(SGSession* session) override;
	void OnSent(SGSession* sessionm, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(SGSession* session, JNetwork::ICommand* cmd) override;
	void OnConnectFailed(SGSession* session, Int32U errorCode) override;
};

