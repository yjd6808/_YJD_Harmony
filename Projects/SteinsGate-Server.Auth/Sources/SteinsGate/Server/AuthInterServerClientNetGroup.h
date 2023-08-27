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
	~AuthInterServerClientNetGroup() override;

	void InitializeParser() override;
	void InitializeIOCP() override;
	void InitializeBufferPool() override;
	void InitializeInterServerTcp() override;
	void InitializeInterServerUdp() override;
protected:
	void OnUpdate(const JCore::TimeSpan& elapsed) override;
};
