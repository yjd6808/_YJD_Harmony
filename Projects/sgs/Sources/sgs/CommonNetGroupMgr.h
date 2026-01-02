/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 3:51:46 PM
 * =====================
 *
 */

#pragma once

#include <jc/Threading/Pulser.h>
#include <jnet/NetGroupMgr.h>
#include <sgs/ServerEnum.h>

class CommonNetGroupMgr : public jnet::NetGroupMgr
{
public:
	CommonNetGroupMgr(int _updatePerSecond);

	void Initialize() override;

	virtual ServerProcessType_t GetProcessType() = 0;

	void SetProcessInfo(ServerProcessInfo* _pProcessInfo);
	ServerProcessInfo* GetProcessInfo() const { return processInfo_; }

	void ProcessMainUpdate();
	void ProcessSubUpdate(const jc::TimeSpan& _elapsed);

	void Terminate()
	{
		running_ = false;
	}

protected:
	virtual void OnUpdate(const jc::TimeSpan& _elapsed) = 0;
	virtual void OnStopped() = 0;

private:
	int updatePerSecond_;
	bool running_;
	ServerProcessInfo* processInfo_;
};
