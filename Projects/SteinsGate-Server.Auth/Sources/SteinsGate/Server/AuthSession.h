/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 12:01:06 AM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/TcpSession.h>
#include <SteinsGate/Server/Auth.h>

class AuthSession : public JNetwork::TcpSession
{
public:
	AuthSession(
		JNetwork::TcpServer* _pServer,
		const JNetwork::IOCPPtr& _pIocp,
		const JCore::MemoryPoolAbstractPtr& _pBufferAllocator,
		int _recvBufferSize,
		int _sendBufferSize
	);
};


