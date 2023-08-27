/*
 * 작성자: 윤정도
 * 생성일: 5/6/2023 8:29:57 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "NetCore.h"

#include <SteinsGate/Client/AuthenticationComponent.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

NetCore::NetCore()
	: m_pNetGroup(nullptr)
	, m_pAuthTcp(nullptr)
	, m_pLobbyTcp(nullptr)
	, m_pLogicTcp(nullptr)
	, m_pLogicUdp(nullptr)
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
	m_pLogicTcp = m_pNetGroup->LogicTcp;
	m_pLogicUdp = m_pNetGroup->LogicUdp;
	m_pChatTcp = m_pNetGroup->ChatTcp;
	m_pAreaTcp = m_pNetGroup->AreaTcp;


	m_pAuthenticationComponent = dbg_new AuthenticationComponent;

	m_ComponentCollection.add(m_pAuthenticationComponent);
}

void NetCore::Finalize() {
	NetMaster::Finalize();

	JCORE_DELETE_SINGLETON_SAFE(m_pCommandSynchronizer);
	JCORE_DELETE_SINGLETON_SAFE(m_pConnectionSynchronizer);

}


void NetCore::pollNetEvents() {
	m_pConnectionSynchronizer->processConnections();
	m_pCommandSynchronizer->processCommands();
}

void NetCore::runCommand(Session* session, ICommand* cmd) {
	if (m_pNetGroup->getParser()->RunCommand(session, cmd) == false) {
		_NetLogWarn_("처리 불가능한 커맨드(%d) 수신", cmd->GetCommand());
	}
}

void NetCore::initializeComponents() {
	m_ComponentCollection.initialize();
}

bool NetCore::connectAuthTcp() {
	if (m_pAuthTcp == nullptr) {
		return false;
	}

	return m_pAuthTcp->ConnectAsync(Core::ServerProcessInfoPackage->Auth.RemoteEP);
}

bool NetCore::connectLobbyTcp() {
	if (m_pLobbyTcp == nullptr) {
		return false;
	}

	return m_pLobbyTcp->ConnectAsync(Core::ServerProcessInfoPackage->Lobby.RemoteEP);
}

