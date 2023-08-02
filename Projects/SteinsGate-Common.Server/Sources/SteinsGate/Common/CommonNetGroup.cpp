/*
 * 작성자: 윤정도
 * 생성일: 4/21/2023 1:52:07 PM
 * =====================
 *
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "CommonNetGroup.h"

#include <SteinsGate/Common/CommonServer.h>

USING_NS_JC;
USING_NS_JNET;


CommonNetGroup::CommonNetGroup()
	: m_pServer(nullptr)
{}

void CommonNetGroup::Initialize() {
	InitializeBufferPool();
	InitializeIOCP();
	InitializeServer();
}

void CommonNetGroup::ProcessUpdate(const JCore::TimeSpan& elapsed) {
	CommonServer* pServer = dynamic_cast<CommonServer*>(m_pServer);

	if (pServer) {
		pServer->ProcessUpdate(elapsed);
	}

	OnUpdate(elapsed);
}


