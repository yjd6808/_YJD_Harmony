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

#include <SteinsGate/Server/AuthServer.h>
#include <SteinsGate/Server/ListenerAuthServer.h>

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

void AuthNetGroup::InitializeServer() {
	auto spServer = MakeShared<AuthServer>(m_spIOCP, m_spBufferPool);

	AddHost(spServer);

	m_pServer = spServer.Get<AuthServer*>();
	m_pServer->SetSesssionContainer(dbg_new SessionContainer(CoreServerProcessInfoPackage_v->Auth.MaxSessionCount));
	m_pServer->SetEventListener(dbg_new ListenerAuthServer);
}

void AuthNetGroup::OnLoop(PulserStatistics* pulserStat) {

}


