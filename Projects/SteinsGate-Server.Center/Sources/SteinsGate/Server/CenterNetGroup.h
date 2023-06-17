/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/InterServerClientNetGroup.h>
#include <SteinsGate/Server/ListenerInterServerClientUdp.h>
#include <SteinsGate/Server/ListenerInterServerHostTcp.h>


class CenterEventListener;
class CenterNetGroup final : public CommonNetGroup
{
public:
	CenterNetGroup();
	~CenterNetGroup() override;
protected:
	void InitializeBufferPool() override;
	void InitializeIOCP() override;
	void InitializeServer() override;
	void OnLoop(JCore::PulserStatistics* pulserStat) override;
private:
	ListenerInterServerHostTcp m_CenterServerEventListener;
	ListenerInterServerClientUdp m_InterServerEventListener;
};


