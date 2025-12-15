/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
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
	void OnConnected(SGSession* _pSession) override;
	void OnDisconnected(SGSession* _pSession) override;
	void OnSent(SGSession* _pSession, JNetwork::ISendPacket* _pSentPacket, Int32UL _sentBytes) override;
	void OnReceived(SGSession* _pSession, JNetwork::ICommand* _pCmd) override;
	void OnConnectFailed(SGSession* _pSession, Int32U _errorCode) override;
};

