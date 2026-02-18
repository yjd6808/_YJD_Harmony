/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:36:49 PM
 * =====================
 *
 */

#pragma once

#include <jc/Container/HashMap.h>

#include <sg/_Net/NetServerListener.h>

#include <sgs/CmdHost.h>
#include <sgs_center/CenterServer.h>

class ListenerCenterServer : public sg::NetServerListener
{
public:
	ListenerCenterServer(CenterServer* _pServer, jnet::CommandParser* _pParser);

protected:
	void OnStarted() override;
	void OnConnected(jnet::Session* _pConnectedSession) override;
	void OnDisconnected(jnet::Session* _pDisconnectedSession, Int32U _errorCode) override;
	void OnSent(jnet::Session* _pSender, jnet::IPacket* _pSentPacket, Int32UL _sentBytes) override;
	void OnReceived(jnet::Session* _pSession, jnet::ICommand* _pRecvCmd) override {}
	void OnReceived(jnet::Session* _pSession, jnet::RecvedCommandPacket* _pRecvPacket) override;
	void OnStopped() override;

	ServerType_t GetServerType() override { return ServerType::Center; }
	void RelayCommand(jnet::ICommand* _pCmd, jnet::RelayCommandBase* _pRelayCmd);

private:
	inline thread_local static jc::HashSet<jnet::Session*> ms_tlsSenderMap;
	CenterServer* centerTcp_;
};
