/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once

#include <sgs/CommonNetGroupMgr.h>

class GameNetMaster final
	: public CommonNetGroupMgr
	, public jc::SingletonPointer<GameNetMaster>
{
	friend class TSingleton;

	GameNetMaster();
	~GameNetMaster() override;
public:
	void Initialize() override;
	ServerProcessType_t GetProcessType() override { return ServerProcessType::Game; }
protected:
	void OnUpdate(const jc::TimeSpan& _elapsed) override;
	void OnStopped() override;
};


