/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 7:47:15 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/NetGroup.h>
#include <JNetwork/Packet/CommandNameDictionary.h>

#include <TF/Server/Host/GameServer.h>
#include <TF/Server/Database/MysqlDatabase.h>

class NetCore : public JNetwork::NetGroup
{
public:
	NetCore();
	~NetCore() override;

	void Initialize() override;
	void ProcessMainLoop();
	void RunCommand(JNetwork::Session* session, JNetwork::ICommand* cmd);
private:
	void Update(const JCore::TimeSpan& elapsed);

	void InitializeBuffer();
	void InitializeIOCP();
	void InitializeHost();
	void InitializeParser();
private:
	bool m_bRunning;

	JNetwork::CommandParser m_Parser;
	GameServer* m_pGameServer;
};
