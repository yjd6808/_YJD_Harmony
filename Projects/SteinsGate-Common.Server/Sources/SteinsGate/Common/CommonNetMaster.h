/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 3:51:46 PM
 * =====================
 *
 */

#pragma once

#include <JCore/Threading/Pulser.h>
#include <JNetwork/NetMaster.h>
#include <SteinsGate/Common/ServerEnum.h>

class CommonNetMaster : public JNetwork::NetMaster
{
public:
	CommonNetMaster(int _updatePerSecond);

	void Initialize() override;

	virtual ServerProcessType_t GetProcessType() = 0;

	void SetProcessInfo(ServerProcessInfo* _pProcessInfo);
	ServerProcessInfo* GetProcessInfo() const { return processInfo_; }

	void ProcessMainUpdate();
	void ProcessSubUpdate(const JCore::TimeSpan& _elapsed);

	void Terminate()
	{
		running_ = false;
	}

protected:
	virtual void OnUpdate(const JCore::TimeSpan& _elapsed) = 0;
	virtual void OnStopped() = 0;

private:
	int updatePerSecond_;
	bool running_;
	ServerProcessInfo* processInfo_;
};
