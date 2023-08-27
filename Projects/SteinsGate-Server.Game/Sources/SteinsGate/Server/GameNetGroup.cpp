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

#include <SteinsGate/Common/CommonSession.h>

#include <SteinsGate/Server/ListenerLogicServer.h>
#include <SteinsGate/Server/ListenerAreaServer.h>
#include <SteinsGate/Server/ListenerChatServer.h>

USING_NS_JC;
USING_NS_JNET;

GameNetGroup::GameNetGroup()
	: m_pLogicTcp(nullptr)
	, m_pAreaTcp(nullptr)
	, m_pChatTcp(nullptr)
{
	SetName("게임 메인");
}

GameNetGroup::~GameNetGroup() {}

SGISessionContainer* GameNetGroup::GetSessionContainer(ServerType_t type) {
	switch (type) {
	case ServerType::Logic: return m_pLogicSessionContainer;
	case ServerType::Chat: return m_pChatSessionContainer;
	case ServerType::Area: return m_pAreaSessionContainer;
	default: DebugAssert(false);
	}
	return nullptr;
}

CommonSession* GameNetGroup::GetSessionFromContainer(int handle) {
	if (Const::Host::LogicHandleRange.Contain(handle)) {
		return dynamic_cast<CommonSession*>(m_pLogicSessionContainer->Get(handle));
	}

	if (Const::Host::AreaHandleRange.Contain(handle)) {
		return dynamic_cast<CommonSession*>(m_pAreaSessionContainer->Get(handle));
	}

	if (Const::Host::ChatHandleRange.Contain(handle)) {
		return dynamic_cast<CommonSession*>(m_pChatSessionContainer->Get(handle));
	}

	DebugAssertMsg(false, "올바르지 않은 세션핸들입니다. (%d)", handle);
	return nullptr;
}

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

	m_pLogicSessionContainer = dbg_new SessionContainer(Core::ServerProcessInfo->MaxSessionCount);
	m_pLogicSessionContainer->SetInitialHandleSeq(Const::Host::LogicHandleRange.Min);
	m_pAreaSessionContainer = dbg_new SessionContainer(Core::ServerProcessInfo->MaxSessionCount);
	m_pAreaSessionContainer->SetInitialHandleSeq(Const::Host::AreaHandleRange.Min);
	m_pChatSessionContainer = dbg_new SessionContainer(Core::ServerProcessInfo->MaxSessionCount);
	m_pChatSessionContainer->SetInitialHandleSeq(Const::Host::ChatHandleRange.Min);

	m_pLogicTcp = spLogicTcp.Get<LogicServer*>();
	m_pLogicTcp->SetSesssionContainer(m_pLogicSessionContainer);
	m_pLogicTcp->SetEventListener(dbg_new ListenerLogicServer{ m_pLogicTcp, m_pParser });

	m_pAreaTcp = spAreaTcp.Get<AreaServer*>();
	m_pAreaTcp->SetSesssionContainer(m_pAreaSessionContainer);
	m_pAreaTcp->SetEventListener(dbg_new ListenerAreaServer{ m_pAreaTcp, m_pParser });

	m_pChatTcp = spChatTcp.Get<ChatServer*>();
	m_pChatTcp->SetSesssionContainer(m_pChatSessionContainer);
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

