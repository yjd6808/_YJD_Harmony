/*
 * 작성자: 윤정도
 * 생성일: 5/12/2023 9:35:47 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Packet/CommandParser.h>
#include <JNetwork/EventListener/ServerEventListener.h>
#include <SteinsGate/Common/ListenerHelper.h>
#include <SteinsGate/Common/Enum.h>

class ListenerCommonServer
	: public JNetwork::ServerEventListener
	, public ListenerHelper
{
protected:
	void OnStarted() override;
	void OnConnected(JNetwork::Session* connectedSession) override;
	void OnDisconnected(JNetwork::Session* disconnetedSession) override;
	void OnSent(JNetwork::Session* sender, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* receiver, JNetwork::ICommand* cmd) override;
	void OnStopped() override;

	virtual ServerType_t GetServerType() = 0;
	const char* GetServerName() { return ServerType::Name[GetServerType()]; }
public:
	JNetwork::CommandParser Parser;
};