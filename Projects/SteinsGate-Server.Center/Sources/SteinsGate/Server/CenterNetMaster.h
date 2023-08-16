/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 3:51:46 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonNetMaster.h>
#include <JCore/Pattern/Singleton.h>

class CenterNetMaster final
	: public CommonNetMaster
	, public JCore::SingletonPointer<CenterNetMaster>
{
private:
	friend class TSingleton;
	CenterNetMaster();
	~CenterNetMaster() override;
public:
	void Initialize() override;
	InterServerClientType_t GetClientType() override { return InterServerClientType::None; }
	ServerProcessType_t GetProcessType() override { return ServerProcessType::Center; }
protected:
	void OnUpdate(const JCore::TimeSpan& elapsed) override;
	void OnStopped() override;
};



