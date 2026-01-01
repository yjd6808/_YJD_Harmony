/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once


#include <sgs/InterServerClientNetGroup.h>

class GameInterServerClientNetGroup : public InterServerClientNetGroup
{
public:
	GameInterServerClientNetGroup();

	void InitializeIOCP() override;
	void InitializeBufferPool() override;
	void InitializeInterServerTcp() override;
	void InitializeInterServerUdp() override;

protected:
	void OnUpdate(const jc::TimeSpan& _elapsed) override;
};
