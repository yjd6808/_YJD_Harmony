/*
 * 작성자: 윤정도
 * 생성일: 5/6/2023 8:29:57 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "NetCore.h"

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

NetCore::NetCore()
	: m_pNetGroup(nullptr)
	, m_pAuthTcp(nullptr)
	, m_pLobbyTcp(nullptr)
	, m_pGameTcp(nullptr)
	, m_pGameUdp(nullptr)
	, m_pChatTcp(nullptr)
	, m_pAreaTcp(nullptr)
	, m_pCommandSynchronizer(CommandSynchronizer::Get())
	, m_pConnectionSynchronizer(ConnectionSynchronizer::Get())
{}

NetCore::~NetCore() {
	NetCore::Finalize();
}

void NetCore::Initialize() {
	const auto spCenterNetGroup = MakeShared<NetClientGroup>();
	AddNetGroup(1, spCenterNetGroup);
	m_pNetGroup = spCenterNetGroup.Get<NetClientGroup*>();
	m_pNetGroup->Initialize();

	m_pCommandSynchronizer->initialize();	// 이녀석은 무조건 IOCP 초기화이후 수행
	m_pConnectionSynchronizer->initialize();

	m_pAuthTcp = m_pNetGroup->AuthTcp;
	m_pLobbyTcp = m_pNetGroup->LobbyTcp;
	m_pGameTcp = m_pNetGroup->GameTcp;
	m_pGameUdp = m_pNetGroup->GameUdp;
	m_pChatTcp = m_pNetGroup->ChatTcp;
	m_pAreaTcp = m_pNetGroup->AreaTcp;
}

void NetCore::Finalize() {
	NetMaster::Finalize();

	DeleteSingletonSafe(m_pCommandSynchronizer);
	DeleteSingletonSafe(m_pConnectionSynchronizer);

}


void NetCore::pollNetEvents() {
	m_pConnectionSynchronizer->processConnections();
	m_pCommandSynchronizer->processCommands();
}

void NetCore::runCommand(ClientConnectServerType_t listenerType, Session* session, ICommand* cmd) {
	if (m_pNetGroup->getParser(listenerType)->RunCommand(session, cmd) == false) {
		_NetLogWarn_("처리 불가능한 커맨드(%d) 수신", cmd->GetCommand());
	}
}

bool NetCore::connectAuthTcp() {
	if (m_pAuthTcp == nullptr) {
		return false;
	}

	return m_pAuthTcp->ConnectAsync(CoreServerProcessInfo_v->Auth.RemoteAuth);
}

