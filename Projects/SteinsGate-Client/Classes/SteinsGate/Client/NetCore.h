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
	void pollNetEvents();
	void runCommand(SGSession* _pSession, JNetwork::ICommand* _pCmd);

	NetClientGroup* getGroup() const { return netClientGroup_; }
	SGTcpClient* getAuthTcp() const { return authTcp_; }
	SGTcpClient* getLobbyTcp() const { return lobbyTcp_; }
	SGTcpClient* getLogicTcp() const { return logicTcp_; }
	SGUdpClient* getLogicUdp() const { return logicUdp_; }
	SGTcpClient* getChatTcp() const { return chatTcp_; }
	SGTcpClient* getAreaTcp() const { return areaTcp_; }

	CommandSynchronizer* getCommandSynchronizer() const { return commandSynchronizer_; }
	ConnectionSynchronizer* getConnectionSynchronizer() const { return connectionSynchronizer_; }

	void initializeComponents();

	AuthenticationComponent* getAuthenticationComponent() const { return authenticationComponent_; }

	bool connectAuthTcp();
	bool connectLobbyTcp();

private:
	NetClientGroup* netClientGroup_;
	SGTcpClient* authTcp_;
	SGTcpClient* lobbyTcp_;
	SGTcpClient* logicTcp_;
	SGUdpClient* logicUdp_;
	SGTcpClient* chatTcp_;
	SGTcpClient* areaTcp_;

	CommandSynchronizer* commandSynchronizer_;
	ConnectionSynchronizer* connectionSynchronizer_;
	ComponentCollection componentCollection_;

	AuthenticationComponent* authenticationComponent_;
};
