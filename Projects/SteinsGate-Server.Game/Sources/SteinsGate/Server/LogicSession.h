/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */

#pragma once

#include <JNetwork/Host/TcpSession.h>
#include <SteinsGate/Server/Game.h>

class LogicSession : public JNetwork::TcpSession
{
public:
	LogicSession(
		JNetwork::TcpServer* _pServer,
		const JNetwork::IOCPPtr& _pIocp,
		const JCore::MemoryPoolAbstractPtr& _pBufferAllocator,
		int _recvBufferSize,
		int _sendBufferSize
	);
};

