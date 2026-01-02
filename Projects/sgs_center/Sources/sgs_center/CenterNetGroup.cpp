/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:49 PM
 * =====================
 *
 */

#include "Core.h"
#include "CenterCoreHeader.h"
#include "CenterNetGroup.h"

#include <jnet/Host/SessionContainer.h>

#include <sgs/CommonSession.h>

#include <sgs_center/ListenerCenterServer.h>
#include <sgs_center/R_MESSAGE.h>
#include <sgs_center/R_PING.h>
#include <sgs_center/R_SETUP.h>

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
jnet::ISessionContainer* CenterNetGroup::GetSessionContainer(ServerType_t _type)
{
	if (_type == ServerType::Center)
	{
		return m_pCenterSessionContainer;
	}

	jc_assert(false);
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
CommonSession* CenterNetGroup::GetSessionFromContainer(int _handle)
{
	if (!Const::Host::LobbyHandleRange.Contain(_handle))
	{
		jc_assert_msg(false, "올바르지 않은 로비 세션핸들입니다. (%d)", _handle);
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
	auto pServer = MakeShared<CenterServer>(pIocp_, pBufferPool_);

	AddHost(Const::Host::CenterTcpId, pServer);

	m_pCenterSessionContainer = dbg_new SessionContainer(sg::ServerProcessInfo->maxSessionCount_);
	m_pCenterSessionContainer->SetInitialHandleSeq(Const::Host::CenterHandleRange.Min);

	m_pCenterTcp = pServer.Get<CenterServer*>();
	m_pCenterTcp->SetSesssionContainer(m_pCenterSessionContainer);
	m_pCenterTcp->SetEventListener(dbg_new ListenerCenterServer{ m_pCenterTcp, pParser_ });

	AddUpdatable(Const::Host::CenterTcpId, m_pCenterTcp);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterNetGroup::OnUpdate(const jc::TimeSpan& _elapsed)
{
}

