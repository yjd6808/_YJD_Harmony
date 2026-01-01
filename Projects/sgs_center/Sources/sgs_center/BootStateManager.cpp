/*
 * 작성자: 윤정도
 * 생성일: 8/22/2023 4:38:40 PM
 * =====================
 *
 */

#include "Core.h"
#include "CenterCoreHeader.h"
#include "BootStateManager.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
void BootStateManager::Initialize()
{
	Arrays::Fill(states_, ServerBootState::Stopped);
}

//////////////////////////////////////////////////////////////////////////////////////////
void BootStateManager::SetState(int _serverId, ServerBootState_t _state)
{
	jc_assert(_serverId >= 0 && _serverId <= Const::Server::MaxProcessId);
	states_[_serverId] = _state;
}

//////////////////////////////////////////////////////////////////////////////////////////
ServerBootState_t BootStateManager::GetState(int _serverId)
{
	jc_assert(_serverId >= 0 && _serverId <= Const::Server::MaxProcessId);
	return states_[_serverId];
}
