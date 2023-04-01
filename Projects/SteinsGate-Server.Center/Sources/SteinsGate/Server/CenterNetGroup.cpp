/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:49 PM
 * =====================
 *
 */

#include "Center.h"
#include "CenterCoreHeader.h"
#include "CenterNetGroup.h"

USING_NS_JC;
USING_NS_JNET;

CenterNetGroup::CenterNetGroup()
	: m_spCenterServer(nullptr)
{}
CenterNetGroup::~CenterNetGroup() {
}

void CenterNetGroup::Initialize() {

	CreateIocp(2);
	CreateBufferPool({});
	RunIocp();

	m_spCenterServer = MakeShared<CenterServer>(m_spIOCP, m_spBufferPool, &m_CenterEventListener, CoreServer_v->Center.MaxSessionCount, 2048, 2048);
	m_spCenterServer->Start(CoreServer_v->Center.BindCenterTcp);

	AddHost(m_spCenterServer);
}

