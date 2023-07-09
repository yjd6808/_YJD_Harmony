/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 11:42:19 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/InterServerClientNetGroup.h>

class AuthInterServerClientNetGroup : public InterServerClientNetGroup
{
public:
	AuthInterServerClientNetGroup();

	void InitializeIOCP() override;
	void InitializeBufferPool() override;
	void InitializeInterServerTcp() override;
	void InitializeInterServerUdp() override;
protected:
	void OnLoop(JCore::PulserStatistics* pulserStat) override;
private:
	ListenerInterServerClientTcp m_InterServerClientTcpEventListener;
	ListenerInterServerClientUdp m_InterServerClientUdpEventListener;
};