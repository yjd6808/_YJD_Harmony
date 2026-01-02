/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */


#pragma once

#include <sgs/CommonNetGroup.h>
#include <sgs_auth/AuthServer.h>

class AuthNetGroup final : public CommonNetGroup
{
public:
	AuthNetGroup();
	~AuthNetGroup() override;

	AuthServer* GetAuthTcp() const { return pAuthTcp_; }
	jnet::ISessionContainer* GetSessionContainer(ServerType_t _type) override;
	jnet::ISessionContainer* GetAuthSessionContainer() const { return pAuthSessionContainer_; }
	CommonSession* GetSessionFromContainer(int _handle) override;

	CommonServer* GetMainTcp() const override { return pAuthTcp_; }
protected:
	void InitializeBufferPool() override;
	void InitializeIOCP() override;
	void InitializeParser() override;
	void InitializeServer() override;

	void OnUpdate(const jc::TimeSpan& _elapsed) override;

	jnet::ISessionContainer* pAuthSessionContainer_;
	AuthServer* pAuthTcp_;
};


