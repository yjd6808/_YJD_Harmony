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

#include <SteinsGate/Common/CommonSession.h>

#include <SteinsGate/Server/ListenerCenterServer.h>
#include <SteinsGate/Server/R_MESSAGE.h>
#include <SteinsGate/Server/R_PING.h>
#include <SteinsGate/Server/R_SETUP.h>

USING_NS_JC;
USING_NS_JNET;

CenterNetGroup::CenterNetGroup()
	: m_pCenterTcp(nullptr)
{
	SetName("센터 메인");
}
CenterNetGroup::~CenterNetGroup() {}

SGISessionContainer* CenterNetGroup::GetSessionContainer(ServerType_t type) {
	if (type == ServerType::Center) {
		return m_pCenterSessionContainer;
	}

	DebugAssert(false);
	return nullptr;
}

CommonSession* CenterNetGroup::GetSessionFromContainer(int handle) {
	if (!Const::Host::LobbyHandleRange.Contain(handle)) {
		DebugAssertMsg(false, "올바르지 않은 로비 세션핸들입니다. (%d)", handle);
		return nullptr;
	}


	return dynamic_cast<CommonSession*>(m_pCenterSessionContainer->Get(handle));
}

void CenterNetGroup::InitializeBufferPool() {
	CreateBufferPool({});
}

void CenterNetGroup::InitializeIOCP() {
	CreateIocp(8);
	RunIocp();
}

void CenterNetGroup::InitializeParser() {
	CommonNetGroup::InitializeParser();

	// SETUP
	m_pParser->AddCommand<SCE_ItsMe>			(R_SETUP::RECV_SCE_ItsMe);
    m_pParser->AddCommand<SCE_NotifyBootState>	(R_SETUP::RECV_SCE_NotifyBootState);

	// MESSAGE
	m_pParser->AddCommand<SS_HostMessage>		(R_MESSAGE::RECV_SS_HostMessage);

	// PING
    m_pParser->AddCommand<SCE_TimeSync>			(R_PING::RECV_SCE_TimeSync);
}

void CenterNetGroup::InitializeServer() {
	auto spServer = MakeShared<CenterServer>(m_spIOCP, m_spBufferPool);

	AddHost(Const::Host::CenterTcpId, spServer);

	m_pCenterSessionContainer = dbg_new SessionContainer(Core::ServerProcessInfo->MaxSessionCount);
	m_pCenterSessionContainer->SetInitialHandleSeq(Const::Host::CenterHandleRange.Min);

	m_pCenterTcp = spServer.Get<CenterServer*>();
	m_pCenterTcp->SetSesssionContainer(m_pCenterSessionContainer);
	m_pCenterTcp->SetEventListener(dbg_new ListenerCenterServer{ m_pCenterTcp, m_pParser });

	AddUpdatable(Const::Host::CenterTcpId, m_pCenterTcp);
}

void CenterNetGroup::OnUpdate(const TimeSpan& elapsed) {
}
