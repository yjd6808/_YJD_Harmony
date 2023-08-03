/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Game.h"
#include "GameCoreHeader.h"
#include "GameInterServerClientNetGroup.h"

#include <SteinsGate/Common/InterServerCmd_HOST.h>

#include <SteinsGate/Server/ListenerInterServerClientTcp.h>
#include <SteinsGate/Server/ListenerInterServerClientUdp.h>

USING_NS_JC;
USING_NS_JNET;

static constexpr int RecvBufferSize_v = 2048;
static constexpr int SendBufferSize_v = 2048;

GameInterServerClientNetGroup::GameInterServerClientNetGroup() {
}

void GameInterServerClientNetGroup::InitializeBufferPool() {
	CreateBufferPool({});
}

void GameInterServerClientNetGroup::InitializeIOCP() {
	CreateIocp(2);
	RunIocp();
}

void GameInterServerClientNetGroup::InitializeInterServerTcp() {
	auto spInterServerClient = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, RecvBufferSize_v, SendBufferSize_v);
	spInterServerClient->Bind(CoreGameServerProcessInfo_v->BindInterServerTcp);
	AddHost(spInterServerClient);

	m_pInterServerClientTcp = spInterServerClient.Get<TcpClient*>();
	m_pInterServerClientTcp->SetEventListener(dbg_new ListenerInterServerClientTcp);
}

void GameInterServerClientNetGroup::InitializeInterServerUdp() {
	auto spInterServerClient = MakeShared<UdpClient>(m_spIOCP, m_spBufferPool, RecvBufferSize_v, SendBufferSize_v);
	spInterServerClient->Bind(CoreGameServerProcessInfo_v->BindInterServerUdp);
	AddHost(spInterServerClient);
	m_pInterServerClientUdp = spInterServerClient.Get<UdpClient*>();
	m_pInterServerClientUdp->SetEventListener(dbg_new ListenerInterServerClientUdp);
	m_pInterServerClientUdp->RecvFromAsync();
}

void GameInterServerClientNetGroup::OnUpdate(const JCore::TimeSpan& elapsed) {
}
