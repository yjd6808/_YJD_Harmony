/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 8:12:38 PM
 * =====================
 *
 */

#include "Pch.h"
#include "GameServer.h"

#include <TF/Server/Host/GameSession.h>

USING_NS_JC;
USING_NS_JNET;

GameServer::GameServer(const IOCPPtr& iocp, const MemoryPoolAbstractPtr& bufferAllocator)
	: TcpServer(iocp, bufferAllocator)
{}

GameServer::~GameServer() {
}

TcpSession* GameServer::CreateSession() {
	return dbg_new GameSession(this, m_spIocp, m_spBufferAllocator);
}

ISessionContainer* GameServer::CreateSessionContainer() {
	return TcpServer::CreateSessionContainer();
}
