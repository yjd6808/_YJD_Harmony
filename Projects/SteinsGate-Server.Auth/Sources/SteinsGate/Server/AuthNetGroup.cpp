/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:49 PM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthNetGroup.h"

#include <SteinsGate/Server/AuthServer.h>

USING_NS_JC;
USING_NS_JNET;

static constexpr int RecvBufferSize_v = 2048;
static constexpr int SendBufferSize_v = 2048;
	

AuthNetGroup::AuthNetGroup() {}
AuthNetGroup::~AuthNetGroup() {}

void AuthNetGroup::InitializeBufferPool() {
	CreateBufferPool({});
}

void AuthNetGroup::InitializeIOCP() {
	CreateIocp(2);
	RunIocp();
}

void AuthNetGroup::InitializeServer() {
	auto spServer = MakeShared<AuthServer>(m_spIOCP, m_spBufferPool, &m_AuthServerEventListener, CoreServerProcessInfo_v->Auth.MaxSessionCount, RecvBufferSize_v, SendBufferSize_v);
	AddHost(spServer);
	m_pServer = spServer.Get<AuthServer*>();
}

void AuthNetGroup::InitializeCenterClient() {
	auto spCenterClient = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, &m_AuthCenterClientEventLIstener, RecvBufferSize_v, SendBufferSize_v);
	spCenterClient->Bind(CoreServerProcessInfo_v->Auth.BindCenterTcp);
	AddHost(spCenterClient);
	m_pCenterClient = spCenterClient.Get<TcpClient*>();
}

void AuthNetGroup::InitializeInterServerClient() {
	auto spInterServerClient = MakeShared<UdpClient>(m_spIOCP, m_spBufferPool, &m_InterServerEventListener, RecvBufferSize_v, SendBufferSize_v);
	spInterServerClient->Bind(CoreServerProcessInfo_v->Auth.BindInterServerUdp);
	AddHost(spInterServerClient);
	m_pInterServerClient = spInterServerClient.Get<UdpClient*>();
}

void AuthNetGroup::OnLoop(int sleepMs) {

}


