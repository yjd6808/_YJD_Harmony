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
	, m_pCenterClient(nullptr)
	, m_pInterServerClient(nullptr)
{}

void CommonNetGroup::Initialize() {
	InitializeBufferPool();
	InitializeIOCP();
	InitializeServer();
	InitializeCenterClient();
	InitializeInterServerClient();
}

bool CommonNetGroup::ConnectCenterServer(int tryCount) {
	DebugAssertMsg(m_pCenterClient != nullptr, "센터 클라이언트가 초기화되어있지 않습니다.");

	constexpr int CenterConnectionTimeout = 1000;

	int iTryCount = 0;

	_LogDebug_("중앙 서버에 접속을 시도합니다...");
	while (!m_pCenterClient->Connect(CoreServerProcessInfo_v->Center.RemoteCenter, CenterConnectionTimeout)) {
		++iTryCount;

		if (iTryCount >= tryCount) {
			break;
		}
	}

	if (m_pCenterClient->GetState() == Host::eConnected) {
		return true;
	}

	_NetLogError_("중앙서버 접속에 실패했습니다. 프로세스를 종료합니다.");
	return false;
}

TcpClient* CommonNetGroup::GetCenterClient() {
	if (CoreCommonNetMaster_v->GetProcessType() == ServerProcessType::Center) {
		_NetLogWarn_("센터서버에서 GetCenterClient() 함수 호출을 시도했습니다.");
		return nullptr;
	}

	return m_pCenterClient;
}

void CommonNetGroup::ProcessLoop(int sleepMs) {
	CommonServer* pServer = dynamic_cast<CommonServer*>(m_pServer);

	if (pServer) {
		pServer->ProcessLoop(sleepMs);
	}

	OnLoop(sleepMs);
}


