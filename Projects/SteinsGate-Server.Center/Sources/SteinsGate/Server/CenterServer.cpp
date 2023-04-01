/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:19:42 PM
 * =====================
 *
 */


#include "Center.h"
#include "CenterCoreHeader.h"
#include "CenterServer.h"

USING_NS_JC;
USING_NS_JNET;

CenterServer::CenterServer(
	const IOCPPtr& iocp, 
	const MemoryPoolAbstractPtr& bufferAllocator,
	TcpServerEventListener* eventListener, 
	int maxConn, 
	int sessionRecvBufferSize,
	int sessionSendBufferSize) : TcpServer(iocp, bufferAllocator, eventListener, maxConn, sessionRecvBufferSize, sessionSendBufferSize)
{}

SGTcpSession* CenterServer::CreateSession() {
	return dbg_new CenterSession(this, m_spIocp, m_spBufferAllocator, m_iSessionRecvBufferSize, m_iSessionSendBufferSize);
}


