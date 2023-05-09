/*
 * 작성자: 윤정도
 * 생성일: 3/29/2023 8:46:53 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/NetClientGroup.h>
#include <SteinsGate/Client/CommandSynchronizer.h>

class NetCore
	: public SGNetMaster
	, public SGSingletonPointer<NetCore>
{
	friend class TSingleton;
	NetCore();
	~NetCore() override;
public:
	void Initialize() override;
	void pollNetEvents();
	void runCommand(JNetwork::ICommand* cmd);

	NetClientGroup* Group;
	SGTcpClient* AuthTcp;
	SGTcpClient* LobbyTcp;
	SGTcpClient* GameTcp;
	SGUdpClient* GameUdp;
	SGTcpClient* AreaTcp;
	SGTcpClient* ChatTcp;
private:
	CommandSynchronizer* m_pCommandSynchronizer;
};



