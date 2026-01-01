/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#include "Core.h"
#include "GameCoreHeader.h"
#include "AreaServer.h"

#include <sgs_game/AreaSession.h>

USING_NS_JC;
USING_NS_JNET;

AreaServer::AreaServer(const IOCPPtr& _pIocp, const MemoryPoolAbstractPtr& _pBufferAllocator)
	: CommonServer(_pIocp, _pBufferAllocator)
{
}
//////////////////////////////////////////////////////////////////////////////////////////
AreaServer::~AreaServer() noexcept
{
}
//////////////////////////////////////////////////////////////////////////////////////////
jnet::TcpSession* AreaServer::CreateSession()
{
	return dbg_new AreaSession(this, pIocp_, pBufferAllocator_, 3072, 3072);
}
//////////////////////////////////////////////////////////////////////////////////////////
void AreaServer::OnUpdate(const TimeSpan& _elapsed)
{
	CommonServer::OnUpdate(_elapsed);
}


