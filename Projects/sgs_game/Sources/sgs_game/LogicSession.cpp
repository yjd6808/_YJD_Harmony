/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "LogicSession.h"

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
LogicSession::LogicSession(
	TcpServer* _pServer,
	const IOCPPtr& _pIocp,
	const jc::MemoryPoolAbstractPtr& _pBufferAllocator,
	int _recvBufferSize,
	int _sendBufferSize
)
	: TcpSession(_pServer, _pIocp, _pBufferAllocator, nullptr, _recvBufferSize, _sendBufferSize)
{
}

