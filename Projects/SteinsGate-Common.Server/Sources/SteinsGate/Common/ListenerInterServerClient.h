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
	ListenerInterServerClient(ServerProcessType_t serverProcessType, SGCommandParser* parser);
protected:
	void OnConnected(JNetwork::Session* session) override;
	void OnDisconnected(JNetwork::Session* session, Int32U errorCode) override;
	void OnSent(JNetwork::Session* sessionm, JNetwork::IPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* session, JNetwork::ICommand* recvCmd) override;
	void OnReceived(JNetwork::Session* session, JNetwork::RecvedCommandPacket* recvPacket) override;
	void OnConnectFailed(JNetwork::Session* session, Int32U errorCode) override;
private:
	ServerProcessType_t m_eServerProcessType;
};

