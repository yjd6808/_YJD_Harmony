/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once

#include <sgs/_Net/NetGroupMgr.h>

class LobbyNetMaster final
	: public NetCore
	, public jc::SingletonPointer<LobbyNetMaster>
{
	friend class TSingleton;

	LobbyNetMaster();
	~LobbyNetMaster() override;
public:
	void Initialize() override;
	ServerProcessType_t GetProcessType() override { return ServerProcessType::Lobby; }
protected:
	void OnUpdate(const jc::TimeSpan& _elapsed) override;
	void OnStopped() override;
};


