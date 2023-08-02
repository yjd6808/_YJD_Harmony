/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 11:44:32 PM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthInterServerClientNetGroup.h"

#include <SteinsGate/Common/InterServerCmd_HOST.h>

#include <SteinsGate/Server/ListenerInterServerClientTcp.h>
#include <SteinsGate/Server/ListenerInterServerClientUdp.h>

USING_NS_JC;
USING_NS_JNET;

static constexpr int RecvBufferSize_v = 2048;
static constexpr int SendBufferSize_v = 2048;

AuthInterServerClientNetGroup::AuthInterServerClientNetGroup() {
}

void AuthInterServerClientNetGroup::InitializeBufferPool() {
	CreateBufferPool({});
}

void AuthInterServerClientNetGroup::InitializeIOCP() {
	CreateIocp(2);
	RunIocp();
}

void AuthInterServerClientNetGroup::InitializeInterServerTcp() {
	auto spInterServerClient = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, RecvBufferSize_v, SendBufferSize_v);
	spInterServerClient->Bind(CoreServerProcessInfoPackage_v->Auth.BindInterServerTcp);
	AddHost(spInterServerClient);

	m_pInterServerClientTcp = spInterServerClient.Get<TcpClient*>();
	m_pInterServerClientTcp->SetEventListener(dbg_new ListenerInterServerClientTcp);
}

void AuthInterServerClientNetGroup::InitializeInterServerUdp() {
	auto spInterServerClient = MakeShared<UdpClient>(m_spIOCP, m_spBufferPool, RecvBufferSize_v, SendBufferSize_v);
	spInterServerClient->Bind(CoreServerProcessInfoPackage_v->Auth.BindInterServerUdp);
	AddHost(spInterServerClient);
	m_pInterServerClientUdp = spInterServerClient.Get<UdpClient*>();
	m_pInterServerClientUdp->SetEventListener(dbg_new ListenerInterServerClientUdp);
	m_pInterServerClientUdp->RecvFromAsync();
}

void AuthInterServerClientNetGroup::OnUpdate(const JCore::TimeSpan& elapsed) {
}
