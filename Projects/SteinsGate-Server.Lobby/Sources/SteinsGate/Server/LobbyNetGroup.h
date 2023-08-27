/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonNetGroup.h>
#include <SteinsGate/Server/LobbyServer.h>

class LobbyNetGroup final : public CommonNetGroup
{
public:
	LobbyNetGroup();
	~LobbyNetGroup() override;

	CommonServer* GetMainTcp() const override { return m_pLobbyTcp; }
	LobbyServer* GetLobbyTcp() const { return m_pLobbyTcp; }
	SGISessionContainer* GetLobbySessionContainer() const { return m_pLobbySessionContainer; }
	SGISessionContainer* GetSessionContainer(ServerType_t type) override;
	CommonSession* GetSessionFromContainer(int handle) override;

protected:
	void InitializeBufferPool() override;
	void InitializeIOCP() override;
	void InitializeParser() override;
	void InitializeServer() override;

	void OnUpdate(const JCore::TimeSpan& elapsed) override;

	LobbyServer* m_pLobbyTcp;
	SGISessionContainer* m_pLobbySessionContainer;
};


