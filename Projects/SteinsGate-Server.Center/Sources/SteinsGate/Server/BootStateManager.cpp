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

//////////////////////////////////////////////////////////////////////////////////////////
void BootStateManager::Initialize()
{
	Arrays::Fill(m_eStates, ServerBootState::Stopped);
}

//////////////////////////////////////////////////////////////////////////////////////////
void BootStateManager::SetState(int _serverId, ServerBootState_t _state)
{
	DebugAssert(_serverId >= 0 && _serverId <= Const::Server::MaxProcessId);
	m_eStates[_serverId] = _state;
}

//////////////////////////////////////////////////////////////////////////////////////////
ServerBootState_t BootStateManager::GetState(int _serverId)
{
	DebugAssert(_serverId >= 0 && _serverId <= Const::Server::MaxProcessId);
	return m_eStates[_serverId];
}
