/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonNetGroup.h>
#include <SteinsGate/Server/CenterServer.h>

class CenterNetGroup final : public CommonNetGroup
{
public:
	CenterNetGroup();
	~CenterNetGroup() override;

	CommonServer* GetMainTcp() const override { return m_pCenterTcp; }
	CenterServer* GetCenterTcp() const { return m_pCenterTcp; }
protected:
	void InitializeBufferPool() override;
	void InitializeIOCP() override;
	void InitializeParser() override;
	void InitializeServer() override;
	void OnUpdate(const JCore::TimeSpan& elapsed) override;

	CenterServer* m_pCenterTcp;
};


