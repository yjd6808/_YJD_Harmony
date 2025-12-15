/*
 * 작성자: 윤정도
 * 생성일: 8/22/2023 4:31:47 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Pattern/Singleton.h>
#include <SteinsGate/Server/Center.h>

class BootStateManager : public JCore::SingletonStaticPointer<BootStateManager>
{
	friend class TSingleton;
	BootStateManager() = default;
	~BootStateManager() = default;
public:
	void Initialize();

	void SetState(int _serverId, ServerBootState_t _state);
	ServerBootState_t GetState(int _serverId);

	ServerBootState_t m_eStates[Const::Server::MaxProcessId];
};




