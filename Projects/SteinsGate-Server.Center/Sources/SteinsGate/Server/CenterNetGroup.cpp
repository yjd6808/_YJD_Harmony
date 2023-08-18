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

#include <SteinsGate/Server/ListenerCenterServer.h>
#include <SteinsGate/Server/R_CENTER.h>

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

void CenterNetGroup::InitializeParser() {
	CommonNetGroup::InitializeParser();

	m_pParser->AddCommand<SCE_ItsMe>			(R_CENTER::RecvItsMe);
    m_pParser->AddCommand<SS_HostMessage>		(R_CENTER::RecvCenterMessage);
    m_pParser->AddCommand<SCE_NotifyBootState>	(R_CENTER::RecvNotifyBootState);
    m_pParser->AddCommand<SCE_TimeSync>			(R_CENTER::RecvTimeSync);
}

void CenterNetGroup::InitializeServer() {
	auto spServer = MakeShared<CenterServer>(m_spIOCP, m_spBufferPool);

	AddHost(spServer);

	m_pServer = spServer.Get<CenterServer*>();
	m_pServer->SetSesssionContainer(dbg_new SessionContainer(CoreServerProcessInfoPackage_v->Center.MaxSessionCount));
	m_pServer->SetEventListener(dbg_new ListenerCenterServer{ m_pParser });
}

void CenterNetGroup::OnUpdate(const TimeSpan& elapsed) {
}
