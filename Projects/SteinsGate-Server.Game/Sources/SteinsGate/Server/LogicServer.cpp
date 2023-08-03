/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Game.h"
#include "GameCoreHeader.h"
#include "LogicServer.h"

#include <SteinsGate/Server/LogicSession.h>

USING_NS_JC;
USING_NS_JNET;

LogicServer::LogicServer(const IOCPPtr& iocp, const MemoryPoolAbstractPtr& bufferAllocator)
	: CommonServer(iocp, bufferAllocator)
{}

SGTcpSession* LogicServer::CreateSession() {
	return dbg_new LogicSession(this, m_spIocp, m_spBufferAllocator, 6144, 6144);
}

ServerInfo LogicServer::GetServerInfo() {
	const GameServerProcessInfo* pProcInfo = (GameServerProcessInfo*)CoreServerProcessInfo_v;
	return { pProcInfo->BindLogicTcp, pProcInfo->MaxSessionCount };
}

void LogicServer::OnUpdate(const TimeSpan& elapsed) {

}


