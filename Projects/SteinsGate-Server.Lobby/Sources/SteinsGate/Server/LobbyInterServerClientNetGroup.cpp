/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "LobbyInterServerClientNetGroup.h"

#include <SteinsGate/Common/InterServerCmd_HOST.h>
#include <SteinsGate/Common/ListenerInterServerClient.h>

USING_NS_JC;
USING_NS_JNET;

static constexpr int RecvBufferSize_v = 2048;
static constexpr int SendBufferSize_v = 2048;

LobbyInterServerClientNetGroup::LobbyInterServerClientNetGroup() {
}

void LobbyInterServerClientNetGroup::InitializeBufferPool() {
	CreateBufferPool({});
}

void LobbyInterServerClientNetGroup::InitializeIOCP() {
	CreateIocp(2);
	RunIocp();
}

void LobbyInterServerClientNetGroup::InitializeInterServerTcp() {
	auto spInterServerClient = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, RecvBufferSize_v, SendBufferSize_v);
	spInterServerClient->Bind(CoreServerProcessInfoPackage_v->Lobby.BindInterServerTcp);
	AddHost(Const::Host::LobbyInterServerTcpId, spInterServerClient);

	m_pInterServerClientTcp = spInterServerClient.Get<TcpClient*>();
	m_pInterServerClientTcp->SetEventListener(dbg_new ListenerInterServerClient{ InterServerClientType::Lobby, m_pParser });
}

void LobbyInterServerClientNetGroup::InitializeInterServerUdp() {
	auto spInterServerClient = MakeShared<UdpClient>(m_spIOCP, m_spBufferPool, RecvBufferSize_v, SendBufferSize_v);
	spInterServerClient->Bind(CoreServerProcessInfoPackage_v->Lobby.BindInterServerUdp);
	AddHost(Const::Host::LobbyInterServerUdpId, spInterServerClient);
	m_pInterServerClientUdp = spInterServerClient.Get<UdpClient*>();
	m_pInterServerClientUdp->SetEventListener(dbg_new ListenerInterServerClient{ InterServerClientType::Lobby, m_pParser });
	m_pInterServerClientUdp->RecvFromAsync();
}

void LobbyInterServerClientNetGroup::OnUpdate(const JCore::TimeSpan& elapsed) {
}
