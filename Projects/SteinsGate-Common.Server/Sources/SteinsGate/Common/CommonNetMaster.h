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

class CommonNetMaster : public JNetwork::NetMaster
{
public:
	CommonNetMaster(int updatePerSecond);

	void Initialize() override;
	virtual InterServerClientType_t GetClientType() = 0;
	virtual ServerProcessType_t GetProcessType() = 0;

	void ProcessMainUpdate();
	void ProcessSubUpdate(const JCore::TimeSpan& elapsed);
	void Terminate() { m_bRunning = false; }
protected:
	virtual void OnUpdate(const JCore::TimeSpan& elapsed) = 0;
	virtual void OnStopped() = 0;
private:
	int m_iUpdatePerSecond;
	bool m_bRunning;
};



