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
	SGISessionContainer* GetSessionSessionContainer() const { return m_pCenterSessionContainer; }
	SGISessionContainer* GetSessionContainer(ServerType_t _type) override;
	CommonSession* GetSessionFromContainer(int _handle) override;
protected:
	void InitializeBufferPool() override;
	void InitializeIOCP() override;
	void InitializeParser() override;
	void InitializeServer() override;
	void OnUpdate(const JCore::TimeSpan& _elapsed) override;

	SGISessionContainer* m_pCenterSessionContainer;
	CenterServer* m_pCenterTcp;
};


