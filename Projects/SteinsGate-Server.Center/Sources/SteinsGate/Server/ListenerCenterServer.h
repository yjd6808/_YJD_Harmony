/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:36:49 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/HashMap.h>

#include <SteinsGate/Common/ListenerServerCommon.h>
#include <SteinsGate/Common/InterServerCmd_HOST.h>


class ListenerCenterServer : public ListenerServerCommon
{
public:
	ListenerCenterServer(SGCommandParser* parser);
protected:
	void OnStarted() override;
	void OnConnected(JNetwork::Session* connectedSession) override;
	void OnDisconnected(JNetwork::Session* disconnetedSession) override;
	void OnSent(JNetwork::Session* sender, JNetwork::ISendPacket* sentPacket, Int32UL sentBytes) override;
	void OnReceived(JNetwork::Session* session, JNetwork::ICommand* recvCmd) override {}
	void OnReceived(JNetwork::Session* session, JNetwork::IRecvPacket* recvPacket) override;
	void OnStopped() override;

	ServerType_t GetServerType() override { return ServerType::Center; }
	void RelayCommand(JNetwork::ICommand* cmd, JNetwork::RelayCommandBase* relayCmd);

private:
	inline thread_local static JCore::HashSet<JNetwork::Session*> ms_tlsSenderMap;
};

