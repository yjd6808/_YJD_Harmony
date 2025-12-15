/*
 * 작성자: 윤정도
 * 생성일: 5/12/2023 9:35:47 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/EventListener/ServerEventListener.h>

class JCORE_NOVTABLE ListenerServerBase : public JNetwork::ServerEventListener
{
public:
	~ListenerServerBase() override = 0;

protected:
	void OnStarted() override;
	void OnConnected(JNetwork::Session* _pConnectedSession) override;
	void OnDisconnected(JNetwork::Session* _pDisconnectedSession, Int32U _errorCode) override;
	void OnSent(JNetwork::Session* _pSender, JNetwork::IPacket* _pSentPacket, Int32UL _sentBytes) override;
	void OnReceived(JNetwork::Session* _pReceiver, JNetwork::ICommand* _pRecvCmd) override;
	void OnReceived(JNetwork::Session* _pReceiver, JNetwork::RecvedCommandPacket* _pRecvPacket) override;
	void OnStopped() override;

	virtual const char* GetServerName() = 0;
};

inline ListenerServerBase::~ListenerServerBase()
{
	/* abstract */
}
