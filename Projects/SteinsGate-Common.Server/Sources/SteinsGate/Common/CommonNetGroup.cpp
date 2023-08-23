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


CommonNetGroup::CommonNetGroup() : m_pParser(dbg_new CommandParser) {}

CommonNetGroup::~CommonNetGroup() {
	JCORE_DELETE_SAFE(m_pParser);
}

void CommonNetGroup::Initialize() {

	if (Core::ServerProcessInfo == nullptr) {
		_LogWarn_("서버 정보가 없어서 네트그룹 초기화 실패 [메인 네트그룹]");
		return;
	}

	InitializeBufferPool();
	InitializeIOCP();
	InitializeParser();
	InitializeServer();
}

void CommonNetGroup::Finalize() {
	NetGroup::Finalize();
	JCORE_DELETE_SAFE(m_pParser);
}

bool CommonNetGroup::AddUpdatable(int id, IUpdatable* updatable) {
	if (!m_UpdatableCollection.Add(id, updatable)) {
		_LogWarn_("넷그룹 업데이터블 추가실패(%d)", id);
		return false;
	}

	return true;
}

void CommonNetGroup::InitializeParser() {
	// 서버 공통 커맨드 정의
}

void CommonNetGroup::ProcessUpdate(const TimeSpan& elapsed) {
	m_UpdatableCollection.Update(elapsed);
}


void CommonNetGroup::ProcessOrder(CenterOrder_t order) {
	switch (order) {
	case CenterOrder::LaunchServer:	
		LaunchServer();
		break;
	case CenterOrder::StopServer:	
		StopServer();
		break;
	default: DebugAssert(false);
	}
}

void CommonNetGroup::LaunchServer() {
	JCORE_LOCK_GUARD(m_ServerBootLock);
	const ServerBootState_t eState = Core::CommonServer->GetBootState();

	if (eState == ServerBootState::Launched || eState == ServerBootState::Launching) {
		return;
	}

	Core::CommonServer->SetBootState(ServerBootState::Launching);
	Core::CommonServer->Start(Core::ServerProcessInfo->BindTcp);
}

void CommonNetGroup::StopServer() {
	JCORE_LOCK_GUARD(m_ServerBootLock);
	const ServerBootState_t eState = Core::CommonServer->GetBootState();

	if (eState == ServerBootState::Stopped || eState == ServerBootState::Stopping) {
		return;
	}

	Core::CommonServer->SetBootState(ServerBootState::Stopping);
	Core::CommonServer->Stop();
}
