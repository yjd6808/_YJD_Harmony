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
	void OnConnected(JNetwork::Session* connectedSession) override;
	void OnDisconnected(JNetwork::Session* disconnetedSession) override;
	void OnSent(JNetwork::Session* sender, JNetwork::IPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* receiver, JNetwork::ICommand* recvCmd) override;
	void OnReceived(JNetwork::Session* receiver, JNetwork::RecvedCommandPacket* recvPacket) override;
	void OnStopped() override;

	virtual const char* GetServerName() = 0;
};

inline ListenerServerBase::~ListenerServerBase() { /* abstract */ }