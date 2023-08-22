/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Game.h"
#include "GameCoreHeader.h"
#include "ChatServer.h"

#include <SteinsGate/Server/ChatSession.h>

USING_NS_JC;
USING_NS_JNET;

ChatServer::ChatServer(const IOCPPtr& iocp, const MemoryPoolAbstractPtr& bufferAllocator)
	: CommonServer(iocp, bufferAllocator)
{}

SGTcpSession* ChatServer::CreateSession() {
	return dbg_new ChatSession(this, m_spIocp, m_spBufferAllocator, 3072, 3072);
}

void ChatServer::OnUpdate(const TimeSpan& elapsed) {
	CommonServer::OnUpdate(elapsed);
}


