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
	, m_eBootState(ServerBootState::Initialized)
	, m_iServerId(InvalidValue_v)
{}

void CenterSession::ConnectedInit() {
	m_eClientType = InterServerClientType::None;
	m_eBootState = ServerBootState::Initialized;
}

void CenterSession::SetClientInformation(InterServerClientType_t type, Int8 serverId) {
	m_eClientType = type;
	m_iServerId = serverId;
}

bool CenterSession::IsValid() const {
	return m_eClientType != InterServerClientType::None && m_iServerId != InvalidValue_v;
}

void CenterSession::SetBootState(ServerBootState_t state) {
	m_eBootState = state;

	if (state == ServerBootState::Launched) {
		_LogInfo_("%s 서버가 실행되었습니다.", InterServerClientType::Name[m_eClientType]);
	} else if (state == ServerBootState::Stopped) {
		_LogInfo_("%s 서버가 중지되었습니다.", InterServerClientType::Name[m_eClientType]);
	}
}


