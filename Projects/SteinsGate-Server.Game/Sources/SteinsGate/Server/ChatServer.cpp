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

//////////////////////////////////////////////////////////////////////////////////////////
ChatServer::ChatServer(const IOCPPtr& _pIocp, const MemoryPoolAbstractPtr& _pBufferAllocator)
	: CommonServer(_pIocp, _pBufferAllocator)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SGTcpSession* ChatServer::CreateSession()
{
	return dbg_new ChatSession(this, pIocp_, pBufferAllocator_, 3072, 3072);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ChatServer::OnUpdate(const TimeSpan& _elapsed)
{
	CommonServer::OnUpdate(_elapsed);
}


