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

class JCORE_NOVTABLE CommonNetMaster : public JNetwork::NetMaster
{
public:
	CommonNetMaster();

	// virtual void Initialize() = 0; 자식에서 구현
	void ProcessMainLoop();
	void ProcessSubLoop(JCore::PulserStatistics* pulseStat);
	void ProcessInputEvent();
	void Terminate() { m_bRunning = false; }

	int GetPulseInterval() { return m_iPulseInterval; }
	virtual CenterClientType_t GetClientType() = 0;
	virtual ServerProcessType_t GetProcessType() = 0;
protected:
	virtual void OnLoop(JCore::PulserStatistics* pulseStat) = 0;
	virtual void OnStopped() = 0;
	virtual void OnCapturedInputEvent(int inputEvent);
private:
	int m_iPulseInterval;
	bool m_bRunning;
	JCore::Vector<int> m_vInputEvents;
};



