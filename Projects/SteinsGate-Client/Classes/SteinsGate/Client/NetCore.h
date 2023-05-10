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
	SGTcpClient* getGameTcp() const { return m_pGameTcp; }
	SGUdpClient* getGameUdp() const { return m_pGameUdp; }
	SGTcpClient* getChatTcp() const { return m_pChatTcp; }
	SGTcpClient* getAreaTcp() const { return m_pAreaTcp; }

	CommandSynchronizer* getCommandSynchronizer() const { return m_pCommandSynchronizer; }
	ConnectionSynchronizer* getConnectionSynchronizer() const { return m_pConnectionSynchronizer; }

	bool connectAuthTcp();
	
private:
	NetClientGroup* m_pNetGroup;
	SGTcpClient* m_pAuthTcp;
	SGTcpClient* m_pLobbyTcp;
	SGTcpClient* m_pGameTcp;
	SGUdpClient* m_pGameUdp;
	SGTcpClient* m_pChatTcp;
	SGTcpClient* m_pAreaTcp;

	CommandSynchronizer* m_pCommandSynchronizer;
	ConnectionSynchronizer* m_pConnectionSynchronizer;
};



