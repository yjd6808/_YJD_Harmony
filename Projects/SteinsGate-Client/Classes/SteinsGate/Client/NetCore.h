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
	void runCommand(ClientConnectServerType_t listenerType, SGSession* session, JNetwork::ICommand* cmd);

	NetClientGroup* getGroup() const { return m_pNetGroup; }
	SGTcpClient* getAuthTcp() const { return m_pAuthTcp; }
	SGTcpClient* getLobbyTcp() const { return m_pLobbyTcp; }
	SGTcpClient* getLogicTcp() const { return m_pLogicTcp; }
	SGUdpClient* getLogicUdp() const { return m_pLogicUdp; }
	SGTcpClient* getChatTcp() const { return m_pChatTcp; }
	SGTcpClient* getAreaTcp() const { return m_pAreaTcp; }

	CommandSynchronizer* getCommandSynchronizer() const { return m_pCommandSynchronizer; }
	ConnectionSynchronizer* getConnectionSynchronizer() const { return m_pConnectionSynchronizer; }

	void initializeComponents();

	AuthenticationComponent* getAuthenticationComponent() const { return m_pAuthenticationComponent; }

	bool connectAuthTcp();
	bool connectLobbyTcp();
	
private:
	NetClientGroup* m_pNetGroup;
	SGTcpClient* m_pAuthTcp;
	SGTcpClient* m_pLobbyTcp;
	SGTcpClient* m_pLogicTcp;
	SGUdpClient* m_pLogicUdp;
	SGTcpClient* m_pChatTcp;
	SGTcpClient* m_pAreaTcp;

	CommandSynchronizer* m_pCommandSynchronizer;
	ConnectionSynchronizer* m_pConnectionSynchronizer;
	ComponentCollection m_ComponentCollection;

	AuthenticationComponent* m_pAuthenticationComponent;
};



