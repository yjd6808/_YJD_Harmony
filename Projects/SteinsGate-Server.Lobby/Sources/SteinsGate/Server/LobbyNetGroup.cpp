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

#include <SteinsGate/Common/Cmd_LOBBY.h>
#include <SteinsGate/Common/CommonSession.h>
#include <SteinsGate/Common/CmdRelay_AUTHENTICATION.h>

#include <SteinsGate/Server/LobbyServer.h>
#include <SteinsGate/Server/ListenerLobbyServer.h>
#include <SteinsGate/Server/R_LOBBY.h>
#include <SteinsGate/Server/R_AUTHENTICATION.h>


USING_NS_JC;
USING_NS_JNET;

LobbyNetGroup::LobbyNetGroup()
	: m_pLobbyTcp(nullptr)
{
	SetName("로비 메인");
}
LobbyNetGroup::~LobbyNetGroup() {}

SGISessionContainer* LobbyNetGroup::GetSessionContainer(ServerType_t type) {
	if (type == ServerType::Lobby) {
		return m_pLobbySessionContainer;
	}

	DebugAssert(false);
	return nullptr;
}

CommonSession* LobbyNetGroup::GetSessionFromContainer(int handle) {
	if (!Const::Host::LobbyHandleRange.Contain(handle)) {
		_LogWarn_("올바르지 않은 로비 세션핸들입니다. (%d)", handle);
		return nullptr;
	}
	

	return dynamic_cast<CommonSession*>(m_pLobbySessionContainer->Get(handle));
}

void LobbyNetGroup::InitializeBufferPool() {
	CreateBufferPool({});
}

void LobbyNetGroup::InitializeIOCP() {
	CreateIocp(2);
	RunIocp();
}

void LobbyNetGroup::InitializeParser() {
	CommonNetGroup::InitializeParser();

	// LOBBY
	m_pParser->AddCommand<CLO_JoinLobby>(R_LOBBY::RECV_CLO_JoinLobby);
}

void LobbyNetGroup::InitializeServer() {
	auto spServer = MakeShared<LobbyServer>(m_spIOCP, m_spBufferPool);

	AddHost(Const::Host::LobbyTcpId, spServer);

	m_pLobbySessionContainer = dbg_new SessionContainer(Core::ServerProcessInfo->MaxSessionCount);
	m_pLobbySessionContainer->SetInitialHandleSeq(Const::Host::LobbyHandleRange.Min);

	m_pLobbyTcp = spServer.Get<LobbyServer*>();
	m_pLobbyTcp->SetSesssionContainer(m_pLobbySessionContainer);
	m_pLobbyTcp->SetEventListener(dbg_new ListenerLobbyServer{ m_pLobbyTcp, m_pParser });

	AddUpdatable(Const::Host::LobbyTcpId, m_pLobbyTcp);
}

void LobbyNetGroup::OnUpdate(const TimeSpan& elapsed) {

}