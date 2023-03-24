/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:49 PM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthNetGroup.h"

USING_NS_JC;
USING_NS_JNET;

AuthNetGroup::AuthNetGroup()
	: m_spAuthServer(nullptr)
{}
AuthNetGroup::~AuthNetGroup() {
}

void AuthNetGroup::Initialize() {

	CreateIocp(2);
	CreateBufferPool({});
	RunIocp();

	m_spAuthServer = MakeShared<AuthServer>(m_spIOCP, m_spBufferPool, &m_AuthEventListener, CoreServer_v->Auth.MaxSessionCount, 2048, 2048);
	m_spAuthServer->Start(CoreServer_v->Auth.BindAuthTcp);

	AddHost(m_spAuthServer);
}

