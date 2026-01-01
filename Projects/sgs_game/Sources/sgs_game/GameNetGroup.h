
/*
 * 
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */

#pragma once

#include <sgs/CommonNetGroup.h>

#include <sgs_game/LogicServer.h>
#include <sgs_game/AreaServer.h>
#include <sgs_game/ChatServer.h>

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

	jnet::ISessionContainer* GetLogicSessionContainer() const { return m_pLogicSessionContainer; }
	jnet::ISessionContainer* GetAreaSessionContainer() const { return m_pChatSessionContainer; }
	jnet::ISessionContainer* GetChatSessionContainer() const { return m_pAreaSessionContainer; }

	jnet::ISessionContainer* GetSessionContainer(ServerType_t _serverType) override;
	CommonSession* GetSessionFromContainer(int _handle) override;

protected:
	void InitializeBufferPool() override;
	void InitializeIOCP() override;
	void InitializeParser() override;
	void InitializeServer() override;
	void OnUpdate(const jc::TimeSpan& _elapsed) override;

	LogicServer* m_pLogicTcp;
	AreaServer* m_pAreaTcp;
	ChatServer* m_pChatTcp;

	jnet::ISessionContainer* m_pLogicSessionContainer;
	jnet::ISessionContainer* m_pChatSessionContainer;
	jnet::ISessionContainer* m_pAreaSessionContainer;
};
