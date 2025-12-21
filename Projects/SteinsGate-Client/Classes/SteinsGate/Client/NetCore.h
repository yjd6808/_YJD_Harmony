/*
 * 작성자: 윤정도
 * 생성일: 3/29/2023 8:46:53 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/NetClientGroup.h>
#include <SteinsGate/Client/CommandSynchronizer.h>
#include <SteinsGate/Client/ConnectionSynchronizer.h>
#include <SteinsGate/Client/ComponentCollection.h>

class AuthenticationComponent;

class NetCore
	: public SGNetMaster
	, public SGSingletonPointer<NetCore>
{
	friend class TSingleton;
	NetCore();
	~NetCore() override;

public:
	void Initialize() override;
	void Finalize() override;
	void PollNetEvents();
	void RunCommand(SGSession* _pSession, JNetwork::ICommand* _pCmd);

	NetClientGroup* GetGroup() const { return pNetClientGroup_; }
	SGTcpClient* GetAuthTcp() const { return pAuthTcp_; }
	SGTcpClient* GetLobbyTcp() const { return pLobbyTcp_; }
	SGTcpClient* GetLogicTcp() const { return pLogicTcp_; }
	SGUdpClient* GetLogicUdp() const { return pLogicUdp_; }
	SGTcpClient* GetChatTcp() const { return pChatTcp_; }
	SGTcpClient* GetAreaTcp() const { return pAreaTcp_; }

	CommandSynchronizer* GetCommandSynchronizer() const { return pCommandSynchronizer_; }
	ConnectionSynchronizer* GetConnectionSynchronizer() const { return pConnectionSynchronizer_; }

	void InitializeComponents();

	AuthenticationComponent* GetAuthenticationComponent() const { return pAuthentication_; }

	bool ConnectAuthTcp();
	bool ConnectLobbyTcp();

private:
	NetClientGroup* pNetClientGroup_;
	SGTcpClient* pAuthTcp_;
	SGTcpClient* pLobbyTcp_;
	SGTcpClient* pLogicTcp_;
	SGUdpClient* pLogicUdp_;
	SGTcpClient* pChatTcp_;
	SGTcpClient* pAreaTcp_;

	CommandSynchronizer* pCommandSynchronizer_;
	ConnectionSynchronizer* pConnectionSynchronizer_;
	ComponentCollection componentCollection_;

	AuthenticationComponent* pAuthentication_;
};
