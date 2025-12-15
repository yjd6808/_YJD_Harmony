/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:36:49 PM
 * =====================
 *
 */

#pragma once

#include <JCore/Container/HashMap.h>

#include <SteinsGate/Common/ListenerServerCommon.h>
#include <SteinsGate/Common/CmdHost.h>
#include <SteinsGate/Server/CenterServer.h>

class ListenerCenterServer : public ListenerServerCommon
{
public:
	ListenerCenterServer(CenterServer* _pServer, SGCommandParser* _pParser);

protected:
	void OnStarted() override;
	void OnConnected(JNetwork::Session* _pConnectedSession) override;
	void OnDisconnected(JNetwork::Session* _pDisconnectedSession, Int32U _errorCode) override;
	void OnSent(JNetwork::Session* _pSender, JNetwork::IPacket* _pSentPacket, Int32UL _sentBytes) override;
	void OnReceived(JNetwork::Session* _pSession, JNetwork::ICommand* _pRecvCmd) override {}
	void OnReceived(JNetwork::Session* _pSession, JNetwork::RecvedCommandPacket* _pRecvPacket) override;
	void OnStopped() override;

	ServerType_t GetServerType() override { return ServerType::Center; }
	void RelayCommand(JNetwork::ICommand* _pCmd, JNetwork::RelayCommandBase* _pRelayCmd);

private:
	inline thread_local static JCore::HashSet<JNetwork::Session*> ms_tlsSenderMap;
	CenterServer* centerTcp_;
};
