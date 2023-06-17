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

#define NETGROUP_ID_MAIN 1				// 각 서버 프로세별로 있는 메인(인증, 중앙, 로비, 게임)서버 그룹 코드
#define NETGROUP_ID_INTERSERVER 2		// 각 서버 프로세별로 있는 인터서버 그룹 코드

class JCORE_NOVTABLE CommonNetMaster : public JNetwork::NetMaster
{
public:
	CommonNetMaster();

	void Initialize() override;
	virtual InterServerClientType_t GetClientType() = 0;
	virtual ServerProcessType_t GetProcessType() = 0;

	void ProcessMainLoop();
	void ProcessSubLoop(JCore::PulserStatistics* pulseStat);
	void ProcessInputEvent();
	void Terminate() { m_bRunning = false; }

	int GetPulseInterval() { return m_iPulseInterval; }
protected:
	virtual void OnLoop(JCore::PulserStatistics* pulseStat) = 0;
	virtual void OnStopped() = 0;
	virtual void OnCapturedInputEvent(int inputEvent);
private:
	int m_iPulseInterval;
	bool m_bRunning;
	JCore::Vector<int> m_vInputEvents;
};



