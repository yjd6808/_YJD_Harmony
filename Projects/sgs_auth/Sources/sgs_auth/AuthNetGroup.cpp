/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:49 PM
 * =====================
 *
 */

#include "Core.h"
#include "AuthCoreHeader.h"
#include "AuthNetGroup.h"

#include <jnet/Host/SessionContainer.h>

#include <sg/Cmd_AUTHENTICATION.h>
#include <sgs/CommonSession.h>
#include <sgs/CmdRelay_AUTHENTICATION.h>

#include <sgs_auth/AuthServer.h>
#include <sgs_auth/ListenerAuthServer.h>
#include <sgs_auth/R_AUTHENTICATION.h>

USING_NS_JC;
USING_NS_JNET;

////////////////////////////////////////////////////////////////////////////////////////////////////
AuthNetGroup::AuthNetGroup()
: pAuthTcp_(nullptr)
{
	SetName("인증 메인");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
AuthNetGroup::~AuthNetGroup()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
jnet::ISessionContainer* AuthNetGroup::GetSessionContainer(ServerType_t _type)
{
	if (_type == ServerType::Auth)
	{
		return pAuthSessionContainer_;
	}

	jc_assert(false);
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
CommonSession* AuthNetGroup::GetSessionFromContainer(int _handle)
{
	if (!Const::Host::AuthHandleRange.Contain(_handle))
	{
		jc_assert_msg(false, "올바르지 않은 로비 세션핸들입니다. (%d)", _handle);
		return nullptr;
	}

	return dynamic_cast<CommonSession*>(pAuthSessionContainer_->Get(_handle));
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
	auto pAuthServer = MakeShared<AuthServer>(pIocp_, pBufferPool_);

	AddHost(Const::Host::AuthTcpId, pAuthServer);

	pAuthSessionContainer_ = dbg_new SessionContainer(sg::ServerProcessInfo->maxSessionCount_);
	pAuthSessionContainer_->SetInitialHandleSeq(Const::Host::AuthHandleRange.Min);

	pAuthTcp_ = pAuthServer.Get<AuthServer*>();
	pAuthTcp_->SetSesssionContainer(pAuthSessionContainer_);
	pAuthTcp_->SetEventListener(dbg_new ListenerAuthServer{ pAuthTcp_, pParser_ });

	AddUpdatable(Const::Host::AuthTcpId, pAuthTcp_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void AuthNetGroup::OnUpdate(const TimeSpan& _elapsed)
{
}

