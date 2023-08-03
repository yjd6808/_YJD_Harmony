/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/InterServerClientNetGroup.h>

class LobbyInterServerClientNetGroup : public InterServerClientNetGroup
{
public:
	LobbyInterServerClientNetGroup();

	void InitializeIOCP() override;
	void InitializeBufferPool() override;
	void InitializeInterServerTcp() override;
	void InitializeInterServerUdp() override;
protected:
	void OnUpdate(const JCore::TimeSpan& elapsed) override;
};
