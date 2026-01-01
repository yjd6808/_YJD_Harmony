/*
 * 작성자: 윤정도
 * 생성일: 5/12/2023 9:35:47 AM
 * =====================
 *
 */


#pragma once

#include <jnet/EventListener/ServerEventListener.h>

class JCORE_NOVTABLE ListenerServerBase : public jnet::ServerEventListener
{
public:
	~ListenerServerBase() override = 0;

protected:
	void OnStarted() override;
	void OnConnected(jnet::Session* _pConnectedSession) override;
	void OnDisconnected(jnet::Session* _pDisconnectedSession, Int32U _errorCode) override;
	void OnSent(jnet::Session* _pSender, jnet::IPacket* _pSentPacket, Int32UL _sentBytes) override;
	void OnReceived(jnet::Session* _pReceiver, jnet::ICommand* _pRecvCmd) override;
	void OnReceived(jnet::Session* _pReceiver, jnet::RecvedCommandPacket* _pRecvPacket) override;
	void OnStopped() override;

	virtual const char* GetServerName() = 0;
};

inline ListenerServerBase::~ListenerServerBase()
{
	/* abstract */
}
