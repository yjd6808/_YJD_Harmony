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
	CommonNetMaster(int updatePerSecond);

	void Initialize() override;

	virtual ServerProcessType_t GetProcessType() = 0;

	void SetProcessInfo(ServerProcessInfo* info);
	ServerProcessInfo* GetProcessInfo() const { return m_pProcessInfo; }

	void ProcessMainUpdate();
	void ProcessSubUpdate(const JCore::TimeSpan& elapsed);
	void Terminate() { m_bRunning = false; }
protected:
	virtual void OnUpdate(const JCore::TimeSpan& elapsed) = 0;
	virtual void OnStopped() = 0;
private:
	int m_iUpdatePerSecond;
	bool m_bRunning;
	ServerProcessInfo* m_pProcessInfo;
};



