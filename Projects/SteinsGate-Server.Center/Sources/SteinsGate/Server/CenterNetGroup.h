/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonNetGroup.h>
#include <SteinsGate/Server/ListenerInterServerClient.h>
#include <SteinsGate/Server/ListenerCenterServer.h>


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
	void InitializeCenterClient() override;
	void InitializeInterServerClient() override;
	void OnLoop(int sleepMs) override;
private:
	ListenerCenterServer m_CenterServerEventListener;
	ListenerInterServerClient m_InterServerEventListener;
};


