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

CenterNetGroup::CenterNetGroup()
	: m_pCenterTcp(nullptr)
{
	SetName("센터 메인");
}
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

	m_pParser->AddCommand<SCE_ItsMe>			(R_CENTER::RECV_SCE_ItsMe);
    m_pParser->AddCommand<SS_HostMessage>		(R_CENTER::RECV_SS_HostMessage);
    m_pParser->AddCommand<SCE_NotifyBootState>	(R_CENTER::RECV_SCE_NotifyBootState);
    m_pParser->AddCommand<SCE_TimeSync>			(R_CENTER::RECV_SCE_TimeSync);
}

void CenterNetGroup::InitializeServer() {
	auto spServer = MakeShared<CenterServer>(m_spIOCP, m_spBufferPool);

	AddHost(Const::Host::CenterTcpId, spServer);

	SessionContainer* pCenterSessionContainer = dbg_new SessionContainer(Core::ServerProcessInfo->MaxSessionCount);
	pCenterSessionContainer->SetInitialHandleSeq(Const::Host::CenterHandleSeq);

	m_pCenterTcp = spServer.Get<CenterServer*>();
	m_pCenterTcp->SetSesssionContainer(pCenterSessionContainer);
	m_pCenterTcp->SetEventListener(dbg_new ListenerCenterServer{ m_pCenterTcp, m_pParser });

	AddUpdatable(Const::Host::CenterTcpId, m_pCenterTcp);
}

void CenterNetGroup::OnUpdate(const TimeSpan& elapsed) {
}
