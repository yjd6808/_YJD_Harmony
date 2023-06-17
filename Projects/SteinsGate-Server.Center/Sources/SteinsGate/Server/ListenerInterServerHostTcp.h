/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:36:49 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/HashMap.h>

#include <SteinsGate/Common/ListenerCommonServer.h>
#include <SteinsGate/Common/InterServerCmd_HOST.h>


class ListenerInterServerHostTcp : public ListenerCommonServer
{
public:
	ListenerInterServerHostTcp();
protected:
	void OnStarted() override;
	void OnConnected(JNetwork::Session* connectedSession) override;
	void OnDisconnected(JNetwork::Session* disconnetedSession) override;
	void OnSent(JNetwork::Session* sender, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* session, JNetwork::ICommand* cmd) override {}
	void OnReceived(JNetwork::Session* session, JNetwork::IRecvPacket* packet) override;
	void OnStopped() override;

	ServerType_t GetServerType() override { return ServerType::Center; }
	void RelayCommand(JNetwork::ICommand* cmd, JNetwork::RelayCommandBase* relayCmd);

private:
	inline thread_local static JCore::HashMap<JNetwork::Session*, JNetwork::Session*> ms_tlsSenderMap;	// TODO: Set 변경필요
};

