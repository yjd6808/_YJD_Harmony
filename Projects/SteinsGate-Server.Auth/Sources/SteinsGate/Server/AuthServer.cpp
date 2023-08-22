/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:19:42 PM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthServer.h"

#include <SteinsGate/Server/AuthSession.h>

USING_NS_JC;
USING_NS_JNET;

AuthServer::AuthServer(const IOCPPtr& iocp, const MemoryPoolAbstractPtr& bufferAllocator)
	: CommonServer(iocp, bufferAllocator)
{}

SGTcpSession* AuthServer::CreateSession() {
	return dbg_new AuthSession(this, m_spIocp, m_spBufferAllocator, 2048, 2048);
}

void AuthServer::OnUpdate(const TimeSpan& elapsed) {
	CommonServer::OnUpdate(elapsed);
}


