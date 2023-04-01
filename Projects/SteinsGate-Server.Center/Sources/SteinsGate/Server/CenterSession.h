/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 12:01:06 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/TcpSession.h>
#include <SteinsGate/Server/Center.h>

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
};


