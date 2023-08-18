/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonNetGroup.h>

class LobbyNetGroup final : public CommonNetGroup
{
public:
	LobbyNetGroup();
	~LobbyNetGroup() override;
protected:
	void InitializeBufferPool() override;
	void InitializeIOCP() override;
	void InitializeParser() override;
	void InitializeServer() override;
	void OnUpdate(const JCore::TimeSpan& elapsed) override;
};


