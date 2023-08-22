/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonNetGroup.h>

#include <SteinsGate/Server/LogicServer.h>
#include <SteinsGate/Server/AreaServer.h>
#include <SteinsGate/Server/ChatServer.h>


class GameNetGroup final : public CommonNetGroup
{
public:
	GameNetGroup();
	~GameNetGroup() override;

	CommonServer* GetMainTcp() const override { return m_pLogicTcp; }

	LogicServer* GetLogicTcp() const { return m_pLogicTcp; }
	AreaServer* GetAreaTcp() const { return m_pAreaTcp; }
	ChatServer* GetChatTcp() const { return m_pChatTcp; }

	void LaunchServer() override;
	void StopServer() override;
protected:
	void InitializeBufferPool() override;
	void InitializeIOCP() override;
	void InitializeParser() override;
	void InitializeServer() override;
	void OnUpdate(const JCore::TimeSpan& elapsed) override;

	LogicServer* m_pLogicTcp;
	AreaServer* m_pAreaTcp;
	ChatServer* m_pChatTcp;
};


