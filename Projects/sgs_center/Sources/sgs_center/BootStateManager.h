/*
 * 작성자: 윤정도
 * 생성일: 8/22/2023 4:31:47 PM
 * =====================
 *
 */


#pragma once



class BootStateManager : public jc::SingletonStaticPointer<BootStateManager>
{
	friend class TSingleton;
	BootStateManager() = default;
	~BootStateManager() = default;
public:
	void Initialize();

	void SetState(int _serverId, ServerBootState_t _state);
	ServerBootState_t GetState(int _serverId);

	ServerBootState_t states_[Const::Server::MaxProcessId];
};




