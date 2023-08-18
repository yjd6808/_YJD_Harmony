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
	ListenerInterServerClient(InterServerClientType_t interServerClientType, SGCommandParser* parser);
protected:
	void OnConnected(JNetwork::Session* session) override;
	void OnDisconnected(JNetwork::Session* session) override;
	void OnSent(JNetwork::Session* sessionm, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* session, JNetwork::ICommand* recvCmd) override;
	void OnReceived(JNetwork::Session* session, JNetwork::IRecvPacket* recvPacket) override;
	void OnConnectFailed(JNetwork::Session* session, Int32U errorCode) override;
private:
	InterServerClientType_t m_eInterServerClientType;
};

