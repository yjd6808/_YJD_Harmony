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

CenterServer::~CenterServer() {
}

SGTcpSession* CenterServer::CreateSession() {
	return dbg_new CenterSession(this, m_spIocp, m_spBufferAllocator, m_iSessionRecvBufferSize, m_iSessionSendBufferSize);
}

ServerInfo CenterServer::GetServerInfo() {
	return { CoreServerProcessInfoPackage_v->Auth.BindAuthTcp, CoreServerProcessInfoPackage_v->Auth.MaxSessionCount };
}

void CenterServer::AddSession(CenterSession* session) {
	DebugAssert(session->IsValid());
	const Int8 id = session->GetServerId();

	m_pSession[id].Session = session;
	m_pSession[id].Type = session->GetClientType();
}

void CenterServer::RemoveSession(CenterSession* session) {
	const Int8 id = session->GetServerId();

	m_pSession[id].Session = nullptr;
	m_pSession[id].Type = InterServerClientType::None;
}

bool CenterServer::IsAllCenterSessionConnected() {
	const Vector<int>& vActiveServerIdList = CoreServerProcessInfoPackage_v->ActiveServerIdList;
	const int iServerCount = vActiveServerIdList.Size();

	for (int i = 0; i < iServerCount; ++i) {
		if (m_pSession[vActiveServerIdList[i]].Session == nullptr) {
			return false;
		}
	}
	return true;
}

bool CenterServer::IsConnected(CenterSession* session) {
	for (int i = 0; i < MaxServerId_v; ++i) {
		if (m_pSession[i].Session == session) {
			return true;
		}
	}
	return false;
}

bool CenterServer::IsConnected(int serverId) {
	if (m_pSession[serverId].Session != nullptr) {
		return true;
	}
	return false;
}

void CenterServer::OnLoop(PulserStatistics* pulserStat) {
}


