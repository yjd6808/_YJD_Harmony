/*
 * 작성자: 윤정도
 * 생성일: 8/22/2023 4:31:47 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Server/Center.h>

class BootStateManager
{
private:
	BootStateManager() = default;
	~BootStateManager() = default;
public:
	void Initialize();

	void SetState(int serverId, ServerBootState_t state);
	ServerBootState_t GetState(int serverId);

	ServerBootState_t m_eStates[Const::Server::MaxId];
	friend class Contents;
};





