/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Core.h"
#include "LobbyCoreHeader.h"
#include "LobbyServer.h"

#include <sgs_lobby/LobbySession.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
LobbyServer::LobbyServer(const IOCPPtr& _pIocp, const MemoryPoolAbstractPtr& _pBufferAllocator)
	: sg::NetServer(_pIocp, _pBufferAllocator)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
jnet::TcpSession* LobbyServer::CreateSession()
{
	return dbg_new LobbySession(this, pIocp_, pBufferAllocator_, 6144, 6144);
}

//////////////////////////////////////////////////////////////////////////////////////////
void LobbyServer::OnUpdate(const TimeSpan& _elapsed)
{
	sg::NetServer::OnUpdate(_elapsed);
}


