/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "LobbyServer.h"

#include <SteinsGate/Server/LobbySession.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
LobbyServer::LobbyServer(const IOCPPtr& _pIocp, const MemoryPoolAbstractPtr& _pBufferAllocator)
	: CommonServer(_pIocp, _pBufferAllocator)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SGTcpSession* LobbyServer::CreateSession()
{
	return dbg_new LobbySession(this, m_spIocp, m_spBufferAllocator, 6144, 6144);
}

//////////////////////////////////////////////////////////////////////////////////////////
void LobbyServer::OnUpdate(const TimeSpan& _elapsed)
{
	CommonServer::OnUpdate(_elapsed);
}


