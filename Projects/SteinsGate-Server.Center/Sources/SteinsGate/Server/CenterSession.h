/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 12:01:06 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/TcpSession.h>
#include <SteinsGate/Common/ServerEnum.h>

class CenterSession : public JNetwork::TcpSession
{
public:
	CenterSession(
		JNetwork::TcpServer* server,
		const JNetwork::IOCPPtr& iocp,
		const JCore::MemoryPoolAbstractPtr& bufferAllocator,
		int recvBufferSize,
		int sendBufferSize
	);

	void ConnectedInit() override;

	CenterClientType_t GetClientType() const { return m_eClientType; }
	void SetClientType(CenterClientType_t type) { m_eClientType = type; }

	ServerBootState_t GetBootState() const { return m_eBootState; }
	void SetBootState(ServerBootState_t state);
private:
	CenterClientType_t m_eClientType;
	ServerBootState_t m_eBootState;
};


