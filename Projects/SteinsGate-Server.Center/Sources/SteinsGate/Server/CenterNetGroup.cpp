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

//////////////////////////////////////////////////////////////////////////////////////////
CenterNetGroup::CenterNetGroup()
	: m_pCenterTcp(nullptr)
{
	SetName("센터 메인");
}

//////////////////////////////////////////////////////////////////////////////////////////
CenterNetGroup::~CenterNetGroup()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SGISessionContainer* CenterNetGroup::GetSessionContainer(ServerType_t _type)
{
	if (_type == ServerType::Center)
	{
		return m_pCenterSessionContainer;
	}

	DebugAssert(false);
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
CommonSession* CenterNetGroup::GetSessionFromContainer(int _handle)
{
	if (!Const::Host::LobbyHandleRange.Contain(_handle))
	{
		DebugAssertMsg(false, "올바르지 않은 로비 세션핸들입니다. (%d)", _handle);
		return nullptr;
	}

	return dynamic_cast<CommonSession*>(m_pCenterSessionContainer->Get(_handle));
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterNetGroup::InitializeBufferPool()
{
	CreateBufferPool({});
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterNetGroup::InitializeIOCP()
{
	CreateIocp(8);
	RunIocp();
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterNetGroup::InitializeParser()
{
	CommonNetGroup::InitializeParser();

	// SETUP
	pParser_->AddCommand<SCE_ItsMe>			(R_SETUP::RECV_SCE_ItsMe);
	pParser_->AddCommand<SCE_NotifyBootState>	(R_SETUP::RECV_SCE_NotifyBootState);

	// MESSAGE
	pParser_->AddCommand<SS_HostMessage>		(R_MESSAGE::RECV_SS_HostMessage);

	// PING
	pParser_->AddCommand<SCE_TimeSync>			(R_PING::RECV_SCE_TimeSync);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterNetGroup::InitializeServer()
{
	auto pServer = MakeShared<CenterServer>(m_spIOCP, m_spBufferPool);

	AddHost(Const::Host::CenterTcpId, pServer);

	m_pCenterSessionContainer = dbg_new SessionContainer(Core::ServerProcessInfo->maxSessionCount_);
	m_pCenterSessionContainer->SetInitialHandleSeq(Const::Host::CenterHandleRange.Min);

	m_pCenterTcp = pServer.Get<CenterServer*>();
	m_pCenterTcp->SetSesssionContainer(m_pCenterSessionContainer);
	m_pCenterTcp->SetEventListener(dbg_new ListenerCenterServer{ m_pCenterTcp, pParser_ });

	AddUpdatable(Const::Host::CenterTcpId, m_pCenterTcp);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterNetGroup::OnUpdate(const JCore::TimeSpan& _elapsed)
{
}

