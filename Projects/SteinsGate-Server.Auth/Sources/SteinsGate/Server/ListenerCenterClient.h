/*
 * 작성자: 윤정도
 * 생성일: 4/5/2023 2:08:13 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ListenerCommonClient.h>

class ListenerCenterClient : public ListenerCommonClient
{
public:
	ListenerCenterClient();
protected:
	void OnConnected(SGSession* session) override;
	void OnDisconnected(SGSession* session) override;
	void OnSent(SGSession* sessionm, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(SGSession* session, JNetwork::ICommand* cmd) override;
	void OnConnectFailed(SGSession* session, Int32U errorCode) override;
};

