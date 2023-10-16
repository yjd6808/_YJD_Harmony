/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 8:12:31 PM
 * =====================
 *
 */


#pragma once


#include <JNetwork/Host/TcpServer.h>

class GameServer : public JNetwork::TcpServer
{
public:
	GameServer(const JNetwork::IOCPPtr& iocp, const JCore::MemoryPoolAbstractPtr& bufferAllocator);
	~GameServer() override;
protected:
	JNetwork::TcpSession* CreateSession() override;
	JNetwork::ISessionContainer* CreateSessionContainer() override;
};


