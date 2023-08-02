/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:49 PM
 * =====================
 *
 */

#include "Center.h"
#include "CenterCoreHeader.h"
#include "CenterNetGroup.h"

#include <JNetwork/Host/SessionContainer.h>
#include <SteinsGate/Server/ListenerInterServerHostTcp.h>


USING_NS_JC;
USING_NS_JNET;

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
	auto spServer = MakeShared<CenterServer>(m_spIOCP, m_spBufferPool);

	AddHost(spServer);

	m_pServer = spServer.Get<CenterServer*>();
	m_pServer->SetSesssionContainer(dbg_new SessionContainer(CoreServerProcessInfoPackage_v->Center.MaxSessionCount));
	m_pServer->SetEventListener(dbg_new ListenerInterServerHostTcp);
}

void CenterNetGroup::OnLoop(PulserStatistics* pulserStat) {
}
