/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 12:01:18 AM
 * =====================
 *
 */

#include "Center.h"
#include "CenterCoreHeader.h"
#include "CenterSession.h"

USING_NS_JC;
USING_NS_JNET;

CenterSession::CenterSession(
	TcpServer* server,
	const IOCPPtr& iocp,
	const JCore::MemoryPoolAbstractPtr& bufferAllocator,
	int recvBufferSize,
	int sendBufferSize)
	: TcpSession(server, iocp, bufferAllocator, recvBufferSize, sendBufferSize)
	, m_eClientType(InterServerClientType::None)
	, m_iServerId(InvalidValue_v)
{}

void CenterSession::OnConnected() {
	m_eClientType = InterServerClientType::None;
}

void CenterSession::OnDisconnected() {

}

void CenterSession::SetClientInformation(InterServerClientType_t type, Int8 serverId) {
	m_eClientType = type;
	m_iServerId = serverId;
}

bool CenterSession::IsValid() const {
	return m_eClientType != InterServerClientType::None && m_iServerId != InvalidValue_v;
}
