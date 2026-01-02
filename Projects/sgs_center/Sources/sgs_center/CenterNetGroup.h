/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */


#pragma once

#include <sgs/CommonNetGroup.h>
#include <sgs_center/CenterServer.h>

class CenterNetGroup final : public CommonNetGroup
{
public:
	CenterNetGroup();
	~CenterNetGroup() override;

	CommonServer* GetMainTcp() const override { return m_pCenterTcp; }
	CenterServer* GetCenterTcp() const { return m_pCenterTcp; }
	jnet::ISessionContainer* GetSessionSessionContainer() const { return m_pCenterSessionContainer; }
	jnet::ISessionContainer* GetSessionContainer(ServerType_t _type) override;
	CommonSession* GetSessionFromContainer(int _handle) override;
protected:
	void InitializeBufferPool() override;
	void InitializeIOCP() override;
	void InitializeParser() override;
	void InitializeServer() override;
	void OnUpdate(const jc::TimeSpan& _elapsed) override;

	jnet::ISessionContainer* m_pCenterSessionContainer;
	CenterServer* m_pCenterTcp;
};


