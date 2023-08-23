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

#include <SteinsGate/Common/Cmd_AUTH.h>

#include <SteinsGate/Server/AuthServer.h>
#include <SteinsGate/Server/ListenerAuthServer.h>
#include <SteinsGate/Server/R_AUTH.h>



USING_NS_JC;
USING_NS_JNET;

AuthNetGroup::AuthNetGroup()
	: m_pAuthTcp(nullptr)
{
	SetName("인증 메인");
}
AuthNetGroup::~AuthNetGroup() {}

void AuthNetGroup::InitializeBufferPool() {
	CreateBufferPool({});
}

void AuthNetGroup::InitializeIOCP() {
	CreateIocp(2);
	RunIocp();
}

void AuthNetGroup::InitializeParser() {
	CommonNetGroup::InitializeParser();

	m_pParser->AddCommand<CAU_Login>(R_AUTH::RECV_CAU_Login);
}

void AuthNetGroup::InitializeServer() {
	auto spServer = MakeShared<AuthServer>(m_spIOCP, m_spBufferPool);

	AddHost(Const::Host::AuthTcpId, spServer);

	SessionContainer* pAuthSessionContainer = dbg_new SessionContainer(Core::ServerProcessInfo->MaxSessionCount);
	pAuthSessionContainer->SetInitialHandleSeq(Const::Host::AuthHandleSeq);

	m_pAuthTcp = spServer.Get<AuthServer*>();
	m_pAuthTcp->SetSesssionContainer(pAuthSessionContainer);
	m_pAuthTcp->SetEventListener(dbg_new ListenerAuthServer{ m_pAuthTcp, m_pParser });

	AddUpdatable(Const::Host::AuthTcpId, m_pAuthTcp);
}

void AuthNetGroup::OnUpdate(const TimeSpan& elapsed) {

}
