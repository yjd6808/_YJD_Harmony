/*
 * 작성자: 윤정도
 * 생성일: 8/22/2023 4:38:40 PM
 * =====================
 *
 */

#include "Center.h"
#include "CenterCoreHeader.h"
#include "BootStateManager.h"

USING_NS_JC;

void BootStateManager::Initialize() {
	Arrays::Fill(m_eStates, ServerBootState::Stopped);
}

void BootStateManager::SetState(int serverId, ServerBootState_t state) {
	DebugAssert(serverId >= 0 && serverId <= Const::Server::MaxProcessId);
	m_eStates[serverId] = state;
}

ServerBootState_t BootStateManager::GetState(int serverId) {
	DebugAssert(serverId >= 0 && serverId <= Const::Server::MaxProcessId);
	return m_eStates[serverId];
}
