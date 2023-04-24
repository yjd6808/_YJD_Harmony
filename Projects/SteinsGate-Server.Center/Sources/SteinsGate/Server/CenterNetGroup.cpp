/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:49 PM
 * =====================
 *
 */

#include "Center.h"
#include "CenterCoreHeader.h"
#include "CenterNetGroup.h"

USING_NS_JC;
USING_NS_JNET;

static constexpr int RecvBufferSize_v = 2048;
static constexpr int SendBufferSize_v = 2048;

CenterNetGroup::CenterNetGroup() {}
CenterNetGroup::~CenterNetGroup() {}

void CenterNetGroup::InitializeBufferPool() {
	CreateBufferPool({});
}

void CenterNetGroup::InitializeIOCP() {
	CreateIocp(8);
	RunIocp();
}

void CenterNetGroup::InitializeServer() {
	auto spServer = MakeShared<CenterServer>(m_spIOCP, m_spBufferPool, &m_CenterServerEventListener, CoreServerProcessInfo_v->Auth.MaxSessionCount, RecvBufferSize_v, SendBufferSize_v);
	AddHost(spServer);
	m_pServer = spServer.Get<CenterServer*>();
}

void CenterNetGroup::InitializeCenterClient() {
	// 중앙서버는 필요없음
}

void CenterNetGroup::InitializeInterServerClient() {
	auto spInterServerClient = MakeShared<UdpClient>(m_spIOCP, m_spBufferPool, &m_InterServerEventListener, RecvBufferSize_v, SendBufferSize_v);
	spInterServerClient->Bind(CoreServerProcessInfo_v->Center.BindInterServerUdp);
	AddHost(spInterServerClient);
	m_pInterServerClient = spInterServerClient.Get<UdpClient*>();
}

void CenterNetGroup::OnLoop(int sleepMs) {
}
