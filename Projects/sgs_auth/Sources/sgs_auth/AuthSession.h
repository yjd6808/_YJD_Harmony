/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 12:01:06 AM
 * =====================
 *
 */


#pragma once

#include <jnet/Host/TcpSession.h>

class AuthSession : public jnet::TcpSession
{
public:
	AuthSession(
		jnet::TcpServer* _pServer,
		const jnet::IOCPPtr& _pIocp,
		const jc::MemoryPoolAbstractPtr& _pBufferAllocator,
		int _recvBufferSize,
		int _sendBufferSize
	);
};


