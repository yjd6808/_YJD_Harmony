/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonNetGroup.h>
#include <SteinsGate/Server/AuthServer.h>

class AuthNetGroup final : public CommonNetGroup
{
public:
	AuthNetGroup();
	~AuthNetGroup() override;

	AuthServer* GetAuthTcp() const { return m_pAuthTcp; }
	SGISessionContainer* GetSessionContainer(ServerType_t type) override;
	SGISessionContainer* GetAuthSessionContainer() const { return m_pAuthSessionContainer; }
	CommonSession* GetSessionFromContainer(int handle) override;

	CommonServer* GetMainTcp() const override { return m_pAuthTcp; }
protected:
	void InitializeBufferPool() override;
	void InitializeIOCP() override;
	void InitializeParser() override;
	void InitializeServer() override;

	void OnUpdate(const JCore::TimeSpan& elapsed) override;

	SGISessionContainer* m_pAuthSessionContainer;
	AuthServer* m_pAuthTcp;
};


