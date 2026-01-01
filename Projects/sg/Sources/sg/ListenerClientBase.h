/*
 * 작성자: 윤정도
 * 생성일: 5/12/2023 9:36:48 AM
 * =====================
 *
 */


#pragma once

#include <jnet/EventListener/ClientEventListener.h>
#include <sg/Enum.h>

class JCORE_NOVTABLE ListenerClientBase : public jnet::ClientEventListener
{
public:
	~ListenerClientBase() override = 0;

protected:
	void OnConnected(jnet::Session* _pSession) override;
	void OnConnectFailed(jnet::Session* _pSession, Int32U _errorCode) override;
	void OnDisconnected(jnet::Session* _pSession, Int32U _errorCode) override;
	void OnSent(jnet::Session* _pSession, jnet::IPacket* _pSentPacket, Int32UL _sentBytes) override;
	void OnReceived(jnet::Session* _pSession, jnet::ICommand* _pRecvCmd) override;
	void OnReceived(jnet::Session* _pSession, jnet::RecvedCommandPacket* _pRecvPacket) override;
};


inline ListenerClientBase::~ListenerClientBase()
{
	/* abstract */
}
