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

AuthServer::AuthServer(
	const IOCPPtr& iocp, 
	const MemoryPoolAbstractPtr& bufferAllocator,
	TcpServerEventListener* eventListener, 
	int maxConn, 
	int sessionRecvBufferSize,
	int sessionSendBufferSize) : CommonServer(iocp, bufferAllocator, eventListener, maxConn, sessionRecvBufferSize, sessionSendBufferSize)
{}

SGTcpSession* AuthServer::CreateSession() {
	return dbg_new AuthSession(this, m_spIocp, m_spBufferAllocator, m_iSessionRecvBufferSize, m_iSessionSendBufferSize);
}

ServerInfo AuthServer::GetServerInfo() {
	return { CoreServerProcessInfo_v->Auth.BindAuthTcp, CoreServerProcessInfo_v->Auth.MaxSessionCount };
}

void AuthServer::OnLoop(int sleepMs) {

}


