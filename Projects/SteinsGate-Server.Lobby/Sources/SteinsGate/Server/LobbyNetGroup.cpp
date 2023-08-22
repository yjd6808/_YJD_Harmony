/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "LobbyNetGroup.h"

#include <JNetwork/Host/SessionContainer.h>

#include <SteinsGate/Server/LobbyServer.h>
#include <SteinsGate/Server/ListenerLobbyServer.h>

USING_NS_JC;
USING_NS_JNET;

LobbyNetGroup::LobbyNetGroup() {}
LobbyNetGroup::~LobbyNetGroup() {}

void LobbyNetGroup::InitializeBufferPool() {
	CreateBufferPool({});
}

void LobbyNetGroup::InitializeIOCP() {
	CreateIocp(2);
	RunIocp();
}

void LobbyNetGroup::InitializeParser() {
	CommonNetGroup::InitializeParser();


}

void LobbyNetGroup::InitializeServer() {
	auto spServer = MakeShared<LobbyServer>(m_spIOCP, m_spBufferPool);

	AddHost(Const::Host::LobbyTcpId, spServer);

	m_pLobbyTcp = spServer.Get<LobbyServer*>();
	m_pLobbyTcp->SetSesssionContainer(dbg_new SessionContainer(CoreServerProcessInfo_v->MaxSessionCount));
	m_pLobbyTcp->SetEventListener(dbg_new ListenerLobbyServer{ m_pLobbyTcp, m_pParser });

	AddUpdatable(Const::Host::LobbyTcpId, m_pLobbyTcp);
}

void LobbyNetGroup::OnUpdate(const TimeSpan& elapsed) {

}