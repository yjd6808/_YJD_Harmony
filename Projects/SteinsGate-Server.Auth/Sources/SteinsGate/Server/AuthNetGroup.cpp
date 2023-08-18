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

AuthNetGroup::AuthNetGroup() {}
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

	m_pParser->AddCommand<CAU_Login>(R_AUTH::RecvLogin);
}

void AuthNetGroup::InitializeServer() {
	auto spServer = MakeShared<AuthServer>(m_spIOCP, m_spBufferPool);

	AddHost(spServer);

	m_pServer = spServer.Get<AuthServer*>();
	m_pServer->SetSesssionContainer(dbg_new SessionContainer(CoreServerProcessInfoPackage_v->Auth.MaxSessionCount));
	m_pServer->SetEventListener(dbg_new ListenerAuthServer{m_pParser});
}

void AuthNetGroup::OnUpdate(const TimeSpan& elapsed) {

}


