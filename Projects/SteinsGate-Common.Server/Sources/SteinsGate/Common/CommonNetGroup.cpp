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
	, m_pParser(dbg_new CommandParser)
{}

void CommonNetGroup::Initialize() {
	InitializeBufferPool();
	InitializeIOCP();
	InitializeParser();
	InitializeServer();
}

void CommonNetGroup::Finalize() {
	NetGroup::Finalize();
	JCORE_DELETE_SAFE(m_pParser);
}

void CommonNetGroup::InitializeParser() {
	// 서버 공통 커맨드 정의
}

void CommonNetGroup::ProcessUpdate(const JCore::TimeSpan& elapsed) {
	CommonServer* pServer = dynamic_cast<CommonServer*>(m_pServer);

	if (pServer) {
		pServer->ProcessUpdate(elapsed);
	}

	OnUpdate(elapsed);
}




