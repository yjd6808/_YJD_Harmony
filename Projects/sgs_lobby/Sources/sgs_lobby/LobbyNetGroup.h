/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once

#include <sgs/_Net/NetGroup_Main.h>
#include <sgs_lobby/LobbyServer.h>

class LobbyNetGroup final : public CommonNetGroup
{
public:
	LobbyNetGroup();
	~LobbyNetGroup() override;

	sg::NetServer* GetMainTcp() const override { return m_pLobbyTcp; }
	LobbyServer* GetLobbyTcp() const { return m_pLobbyTcp; }
	jnet::ISessionContainer* GetLobbySessionContainer() const { return m_pLobbySessionContainer; }
	jnet::ISessionContainer* GetSessionContainer(ServerType_t _type) override;
	sg::NetSession* GetSessionFromContainer(int _handle) override;

protected:
	void InitializeBufferPool() override;
	void InitializeIOCP() override;
	void InitializeParser() override;
	void InitializeServer() override;

	void OnUpdate(const jc::TimeSpan& _elapsed) override;

	LobbyServer* m_pLobbyTcp;
	jnet::ISessionContainer* m_pLobbySessionContainer;
};


