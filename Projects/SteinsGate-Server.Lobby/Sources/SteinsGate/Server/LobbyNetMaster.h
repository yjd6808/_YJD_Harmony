/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonNetMaster.h>

class LobbyNetMaster final
	: public CommonNetMaster
	, public SGSingletonPointer<LobbyNetMaster>
{
	friend class TSingleton;

	LobbyNetMaster();
	~LobbyNetMaster() override;
public:
	void Initialize() override;
	InterServerClientType_t GetClientType() override { return InterServerClientType::Lobby; }
	ServerProcessType_t GetProcessType() override { return ServerProcessType::Lobby; }
protected:
	void OnUpdate(const JCore::TimeSpan& elapsed) override;
	void OnStopped() override;
};



