/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Game.h"
#include "GameCoreHeader.h"
#include "GameNetGroup.h"

#include <JNetwork/Host/SessionContainer.h>

#include <SteinsGate/Server/ListenerLogicServer.h>
#include <SteinsGate/Server/ListenerAreaServer.h>
#include <SteinsGate/Server/ListenerChatServer.h>

#include "SteinsGate/Common/S_INTERSERVER_COMMON.h"

USING_NS_JC;
USING_NS_JNET;

GameNetGroup::GameNetGroup()
	: m_pLogicTcp(nullptr)
	, m_pAreaTcp(nullptr)
	, m_pChatTcp(nullptr)
{}
GameNetGroup::~GameNetGroup() {}

void GameNetGroup::InitializeBufferPool() {
	CreateBufferPool({});
}

void GameNetGroup::InitializeIOCP() {
	CreateIocp(2);
	RunIocp();
}

void GameNetGroup::InitializeParser() {
	CommonNetGroup::InitializeParser();

	// 로직서버 관련

	// 지역서버 관련

	// 채팅서버 관련
}

void GameNetGroup::InitializeServer() {
	auto spLogicTcp = MakeShared<LogicServer>(m_spIOCP, m_spBufferPool);
	auto spAreaTcp = MakeShared<AreaServer>(m_spIOCP, m_spBufferPool);
	auto spChatTcp = MakeShared<ChatServer>(m_spIOCP, m_spBufferPool);

	AddHost(Const::Host::LogicTcpId, spLogicTcp);
	AddHost(Const::Host::AreaTcpId, spAreaTcp);
	AddHost(Const::Host::ChatTcpId, spChatTcp);

	SessionContainer* pLogicSessionContainer = dbg_new SessionContainer(Core::ServerProcessInfo->MaxSessionCount);
	SessionContainer* pAreaSessionContainer = dbg_new SessionContainer(Core::ServerProcessInfo->MaxSessionCount);
	SessionContainer* pChatSessionContainer = dbg_new SessionContainer(Core::ServerProcessInfo->MaxSessionCount);

	pLogicSessionContainer->SetInitialHandleSeq(Const::Host::LogicHandleSeq);
	pAreaSessionContainer->SetInitialHandleSeq(Const::Host::AreaHandleSeq);
	pChatSessionContainer->SetInitialHandleSeq(Const::Host::ChatHandleSeq);

	m_pLogicTcp = spLogicTcp.Get<LogicServer*>();
	m_pLogicTcp->SetSesssionContainer(pLogicSessionContainer);
	m_pLogicTcp->SetEventListener(dbg_new ListenerLogicServer{ m_pLogicTcp, m_pParser });

	m_pAreaTcp = spAreaTcp.Get<AreaServer*>();
	m_pAreaTcp->SetSesssionContainer(pAreaSessionContainer);
	m_pAreaTcp->SetEventListener(dbg_new ListenerAreaServer{ m_pAreaTcp, m_pParser });

	m_pChatTcp = spChatTcp.Get<ChatServer*>();
	m_pChatTcp->SetSesssionContainer(pChatSessionContainer);
	m_pChatTcp->SetEventListener(dbg_new ListenerChatServer{ m_pChatTcp, m_pParser });

	AddUpdatable(Const::Host::LogicTcpId, m_pLogicTcp);
	AddUpdatable(Const::Host::AreaTcpId, m_pAreaTcp);
	AddUpdatable(Const::Host::ChatTcpId, m_pChatTcp);
}

void GameNetGroup::OnUpdate(const TimeSpan& elapsed) {

}

void GameNetGroup::LaunchServer() {
	JCORE_LOCK_GUARD(m_ServerBootLock);

	static CommonServer* s_Servers[3];
	static CommonServer* s_FailedServer[3];
	static SGEndPoint s_BindEP[3];
	int iFailedCount = 0;

	s_Servers[0] = m_pChatTcp;
	s_Servers[1] = m_pAreaTcp;
	s_Servers[2] = m_pLogicTcp;
	s_BindEP[0] = Core::GameServerProcessInfo->BindChatTcp;
	s_BindEP[1] = Core::GameServerProcessInfo->BindAreaTcp;
	s_BindEP[2] = Core::GameServerProcessInfo->BindTcp;
	s_FailedServer[0] = nullptr;
	s_FailedServer[1] = nullptr;
	s_FailedServer[2] = nullptr;

	for (int i = 0; i < 3; ++i) {
		CommonServer* pServer = s_Servers[i];
		const ServerBootState_t eState = pServer->GetBootState();

		if (eState == ServerBootState::Launched || eState == ServerBootState::Launching) {
			continue;
		}

		pServer->SetBootState(ServerBootState::Launching);
		if (!pServer->Start(s_BindEP[i]))
			s_FailedServer[iFailedCount++] = pServer;
	}
}

void GameNetGroup::StopServer() {
	JCORE_LOCK_GUARD(m_ServerBootLock);

	static CommonServer* s_Servers[3];
	static CommonServer* s_FailedServer[3];
	int iFailedCount = 0;

	s_Servers[0] = m_pChatTcp;
	s_Servers[1] = m_pAreaTcp;
	s_Servers[2] = m_pLogicTcp;
	s_FailedServer[0] = nullptr;
	s_FailedServer[1] = nullptr;
	s_FailedServer[2] = nullptr;

	for (int i = 0; i < 3; ++i) {
		CommonServer* pServer = s_Servers[i];
		const ServerBootState_t eState = pServer->GetBootState();

		if (eState == ServerBootState::Stopped || eState == ServerBootState::Stopping) {
			continue;
		}

		pServer->SetBootState(ServerBootState::Stopping);
		if (!pServer->Stop())
			s_FailedServer[iFailedCount++] = pServer;
	}
}

