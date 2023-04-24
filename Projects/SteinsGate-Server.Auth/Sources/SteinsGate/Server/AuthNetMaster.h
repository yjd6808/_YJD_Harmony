/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 3:51:46 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonNetMaster.h>
#include <JCore/Pattern/Singleton.h>

class AuthNetMaster final : public CommonNetMaster, public JCore::SingletonPointer<AuthNetMaster>
{
	friend class TSingleton;

	AuthNetMaster();
	~AuthNetMaster() override;
public:
	void Initialize() override;
	CenterClientType_t GetClientType() override { return CenterClientType::Auth; }
	ServerProcessType_t GetProcessType() override { return ServerProcessType::Auth; }
protected:
	void OnLoop(int sleepMs) override;
	void OnCapturedInputEvent(int inputEvent) override;
	void OnStopped() override;
};



