/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:14 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonNetGroup.h>

class AuthNetGroup final : public CommonNetGroup
{
public:
	AuthNetGroup();
	~AuthNetGroup() override;
protected:
	void InitializeBufferPool() override;
	void InitializeIOCP() override;
	void InitializeParser() override;
	void InitializeServer() override;

	void OnUpdate(const JCore::TimeSpan& elapsed) override;
};


