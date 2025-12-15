/*
 * 작성자: 윤정도
 * 생성일: 5/12/2023 9:36:48 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/EventListener/ClientEventListener.h>
#include <SteinsGate/Common/Enum.h>

class JCORE_NOVTABLE ListenerClientBase : public JNetwork::ClientEventListener
{
public:
	~ListenerClientBase() override = 0;

protected:
	void OnConnected(JNetwork::Session* _pSession) override;
	void OnConnectFailed(JNetwork::Session* _pSession, Int32U _errorCode) override;
	void OnDisconnected(JNetwork::Session* _pSession, Int32U _errorCode) override;
	void OnSent(JNetwork::Session* _pSession, JNetwork::IPacket* _pSentPacket, Int32UL _sentBytes) override;
	void OnReceived(JNetwork::Session* _pSession, JNetwork::ICommand* _pRecvCmd) override;
	void OnReceived(JNetwork::Session* _pSession, JNetwork::RecvedCommandPacket* _pRecvPacket) override;
};


inline ListenerClientBase::~ListenerClientBase()
{
	/* abstract */
}
