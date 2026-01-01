/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Core.h"
#include "GameCoreHeader.h"
#include "LogicServer.h"

#include <sgs_game/LogicSession.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
LogicServer::LogicServer(const IOCPPtr& _pIocp, const MemoryPoolAbstractPtr& _pBufferAllocator)
	: CommonServer(_pIocp, _pBufferAllocator)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
jnet::TcpSession* LogicServer::CreateSession()
{
	return dbg_new LogicSession(this, pIocp_, pBufferAllocator_, 6144, 6144);
}

//////////////////////////////////////////////////////////////////////////////////////////
void LogicServer::OnUpdate(const TimeSpan& _elapsed)
{
	CommonServer::OnUpdate(_elapsed);
}
