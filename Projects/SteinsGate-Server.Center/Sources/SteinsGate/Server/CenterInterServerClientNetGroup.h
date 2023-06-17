/*
 * 작성자: 윤정도
 * 생성일: 6/16/2023 11:42:19 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/InterServerClientNetGroup.h>
#include <SteinsGate/Server/ListenerInterServerClientUdp.h>

class CenterInterServerClientNetGroup : public InterServerClientNetGroup
{
public:
	CenterInterServerClientNetGroup();

	void InitializeIOCP() override;
	void InitializeBufferPool() override;
	void InitializeInterServerTcp() override;
	void InitializeInterServerUdp() override;
protected:
	void OnLoop(JCore::PulserStatistics* pulseStat) override;
private:
	ListenerInterServerClientUdp m_InterServerClientUdpListener;
};
