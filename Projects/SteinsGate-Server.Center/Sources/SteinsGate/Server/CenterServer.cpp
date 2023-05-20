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
	ServerEventListener* eventListener, 
	int maxConn, 
	int sessionRecvBufferSize,
	int sessionSendBufferSize)
	: CommonServer(iocp, bufferAllocator, eventListener, maxConn, sessionRecvBufferSize, sessionSendBufferSize)
	, m_bStartupLaunching(true)
{}

SGTcpSession* CenterServer::CreateSession() {
	return dbg_new CenterSession(this, m_spIocp, m_spBufferAllocator, m_iSessionRecvBufferSize, m_iSessionSendBufferSize);
}

ServerInfo CenterServer::GetServerInfo() {
	return { CoreServerProcessInfo_v->Auth.BindAuthTcp, CoreServerProcessInfo_v->Auth.MaxSessionCount };
}

bool CenterServer::IsAllCenterSessionConnected() {
	for (int i = CenterClientType::Begin; i <= CenterClientType::End; ++i) {
		if (m_pSession[i] == nullptr) {
			return false;
		}
	}
	return true;
}

bool CenterServer::IsConnected(CenterSession* session) {
	for (int i = CenterClientType::Begin; i <= CenterClientType::End; ++i) {
		if (m_pSession[i] == session) {
			return true;
		}
	}
	return false;
}

void CenterServer::OnLoop(PulserStatistics* pulserStat) {
}


