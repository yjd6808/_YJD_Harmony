/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 3:51:46 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/NetMaster.h>
#include <SteinsGate/Common/ServerEnum.h>

class CommonNetMaster : public JNetwork::NetMaster
{
public:
	CommonNetMaster();

	void ProcessMainLoop();
	void ProcessSubLoop(int sleepMs);
	void ProcessInputEvent();
	void Terminate() { m_bRunning = false; }

	int GetPulseInterval() { return m_iPulseInterval; }
	virtual CenterClientType_t GetClientType() = 0;
	virtual ServerProcessType_t GetProcessType() = 0;
protected:
	virtual void OnLoop(int sleepMs) = 0;
	virtual void OnStopped() = 0;
	virtual void OnCapturedInputEvent(int inputEvent);
private:
	int m_iPulseInterval;
	bool m_bRunning;
	JCore::Vector<int> m_vInputEvents;
};



