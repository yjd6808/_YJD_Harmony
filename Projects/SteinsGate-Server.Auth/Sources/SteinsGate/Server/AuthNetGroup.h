/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonNetGroup.h>

#include <SteinsGate/Server/ListenerAuthServer.h>
#include <SteinsGate/Server/ListenerCenterClient.h>
#include <SteinsGate/Server/ListenerInterServerClient.h>


class ListenerCenterServer;
class AuthNetGroup final : public CommonNetGroup
{
public:
	AuthNetGroup();
	~AuthNetGroup() override;
protected:
	void InitializeBufferPool() override;
	void InitializeIOCP() override;
	void InitializeServer() override;
	void InitializeCenterClient() override;
	void InitializeInterServerClient() override;
	void OnLoop(JCore::PulserStatistics* pulserStat) override;
private:
	ListenerAuthServer m_AuthServerEventListener;
	ListenerCenterClient m_AuthCenterClientEventLIstener;
	ListenerInterServerClient m_InterServerEventListener;
};


