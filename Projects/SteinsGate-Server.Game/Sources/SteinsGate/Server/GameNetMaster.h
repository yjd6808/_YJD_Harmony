/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonNetMaster.h>

class GameNetMaster final
	: public CommonNetMaster
	, public SGSingletonPointer<GameNetMaster>
{
	friend class TSingleton;

	GameNetMaster();
	~GameNetMaster() override;
public:
	void Initialize() override;
	InterServerClientType_t GetClientType() override { return InterServerClientType::Game; }
	ServerProcessType_t GetProcessType() override { return ServerProcessType::Game; }
protected:
	void OnUpdate(const JCore::TimeSpan& elapsed) override;
	void OnCapturedInputEvent(int inputEvent) override;
	void OnStopped() override;
};



