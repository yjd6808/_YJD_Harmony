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

#include <SteinsGate/Server/LogicServer.h>
#include <SteinsGate/Server/ListenerLogicServer.h>

USING_NS_JC;
USING_NS_JNET;

GameNetGroup::GameNetGroup() {}
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


}

void GameNetGroup::InitializeServer() {
	auto spServer = MakeShared<LogicServer>(m_spIOCP, m_spBufferPool);

	AddHost(spServer);

	m_pServer = spServer.Get<LogicServer*>();
	m_pServer->SetSesssionContainer(dbg_new SessionContainer(CoreGameServerProcessInfo_v->MaxSessionCount));
	m_pServer->SetEventListener(dbg_new ListenerLogicServer{ m_pParser });
}

void GameNetGroup::OnUpdate(const TimeSpan& elapsed) {

}


