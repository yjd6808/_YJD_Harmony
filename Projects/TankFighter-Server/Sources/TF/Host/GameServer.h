#pragma once



#include <TF/ServerConfiguration.h>
#include <TF/Game/Player.h>
#include <TF/Parser/CommandParser.h>
#include <JNetwork/Host/TcpServer.h>
#include <JNetwork/Host/TcpSession.h>


class GameServer : public JNetwork::TcpServer
{
public:
	GameServer() : TcpServer(TCP_MAXIMUM_CONNECTION) {}

	static GameServer* GetInstance();
	~GameServer() override;
public:
	bool StartServer();
private:
	CommandParser m_Parser;
	JNetwork::TcpServerEventListener* m_pListener = nullptr;
	static GameServer* ms_pInstance;
};

