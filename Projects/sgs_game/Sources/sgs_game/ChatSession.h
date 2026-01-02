/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */

#pragma once

#include <jnet/Host/TcpSession.h>

class ChatSession : public jnet::TcpSession
{
public:
	ChatSession(
		jnet::TcpServer* _pServer,
		const jnet::IOCPPtr& _pIocp,
		const jc::MemoryPoolAbstractPtr& _pBufferAllocator,
		int _recvBufferSize,
		int _sendBufferSize
	);
};


