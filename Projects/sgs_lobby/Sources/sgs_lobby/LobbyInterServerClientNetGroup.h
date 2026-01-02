/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once


#include <sgs/InterServerClientNetGroup.h>

class LobbyInterServerClientNetGroup : public InterServerClientNetGroup
{
public:
	LobbyInterServerClientNetGroup();

	void InitializeBufferPool() override;
	void InitializeIOCP() override;
	void InitializeParser() override;
	void InitializeInterServerTcp() override;
	void InitializeInterServerUdp() override;

protected:
	void OnUpdate(const jc::TimeSpan& _elapsed) override;
};
