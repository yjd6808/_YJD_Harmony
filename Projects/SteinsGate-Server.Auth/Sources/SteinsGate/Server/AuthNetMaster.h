/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 3:51:46 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonNetMaster.h>

class AuthNetMaster final
	: public CommonNetMaster
	, public JCore::SingletonPointer<AuthNetMaster>
{
	friend class TSingleton;
	AuthNetMaster();
	~AuthNetMaster() override;
public:
	void Initialize() override;
	ServerProcessType_t GetProcessType() override { return ServerProcessType::Auth; }
protected:
	void OnUpdate(const JCore::TimeSpan& elapsed) override;
	void OnStopped() override;
};



