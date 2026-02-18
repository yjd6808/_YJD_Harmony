/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:19:42 PM
 * =====================
 *
 */


#include "Core.h"
#include "AuthCoreHeader.h"
#include "AuthServer.h"

#include <sgs_auth/AuthSession.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
AuthServer::AuthServer(const IOCPPtr& _pIocp, const MemoryPoolAbstractPtr& _pBufferAllocator)
: sg::NetServer(_pIocp, _pBufferAllocator)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
jnet::TcpSession* AuthServer::CreateSession()
{
	return dbg_new AuthSession(this, pIocp_, pBufferAllocator_, 2048, 2048);
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthServer::OnUpdate(const TimeSpan& _elapsed)
{
	sg::NetServer::OnUpdate(_elapsed);
}
