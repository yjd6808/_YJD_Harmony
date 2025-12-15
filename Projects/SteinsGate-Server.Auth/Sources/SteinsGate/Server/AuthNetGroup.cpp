/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:49 PM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthNetGroup.h"

#include <JNetwork/Host/SessionContainer.h>

#include <SteinsGate/Common/Cmd_AUTHENTICATION.h>
#include <SteinsGate/Common/CommonSession.h>
#include <SteinsGate/Common/CmdRelay_AUTHENTICATION.h>

#include <SteinsGate/Server/AuthServer.h>
#include <SteinsGate/Server/ListenerAuthServer.h>
#include <SteinsGate/Server/R_AUTHENTICATION.h>

USING_NS_JC;
USING_NS_JNET;

////////////////////////////////////////////////////////////////////////////////////////////////////
AuthNetGroup::AuthNetGroup()
	: m_pAuthTcp(nullptr)
{
	SetName("인증 메인");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
AuthNetGroup::~AuthNetGroup()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
SGISessionContainer* AuthNetGroup::GetSessionContainer(ServerType_t _type)
{
	if (_type == ServerType::Auth)
	{
		return m_pAuthSessionContainer;
	}

	DebugAssert(false);
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
CommonSession* AuthNetGroup::GetSessionFromContainer(int _handle)
{
	if (!Const::Host::AuthHandleRange.Contain(_handle))
	{
		DebugAssertMsg(false, "올바르지 않은 로비 세션핸들입니다. (%d)", _handle);
		return nullptr;
	}

	return dynamic_cast<CommonSession*>(m_pAuthSessionContainer->Get(_handle));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void AuthNetGroup::InitializeBufferPool()
{
	CreateBufferPool({});
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void AuthNetGroup::InitializeIOCP()
{
	CreateIocp(2);
	RunIocp();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void AuthNetGroup::InitializeParser()
{
	CommonNetGroup::InitializeParser();

	// AUTHENTICATION
	pParser_->AddCommand<CAU_Login>(R_AUTHENTICATION::RECV_CAU_Login);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void AuthNetGroup::InitializeServer()
{
	auto pAuthServer = MakeShared<AuthServer>(m_spIOCP, m_spBufferPool);

	AddHost(Const::Host::AuthTcpId, pAuthServer);

	m_pAuthSessionContainer = dbg_new SessionContainer(Core::ServerProcessInfo->maxSessionCount_);
	m_pAuthSessionContainer->SetInitialHandleSeq(Const::Host::AuthHandleRange.Min);

	m_pAuthTcp = pAuthServer.Get<AuthServer*>();
	m_pAuthTcp->SetSesssionContainer(m_pAuthSessionContainer);
	m_pAuthTcp->SetEventListener(dbg_new ListenerAuthServer{ m_pAuthTcp, pParser_ });

	AddUpdatable(Const::Host::AuthTcpId, m_pAuthTcp);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void AuthNetGroup::OnUpdate(const TimeSpan& _elapsed)
{
}

