/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 12:01:18 AM
 * =====================
 *
 */

#include "Core.h"
#include "AuthCoreHeader.h"
#include "AuthSession.h"

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
AuthSession::AuthSession(
	TcpServer* _pServer,
	const IOCPPtr& _pIocp,
	const jc::MemoryPoolAbstractPtr& _pBufferAllocator,
	int _recvBufferSize,
	int _sendBufferSize
) : TcpSession(_pServer, _pIocp, _pBufferAllocator, nullptr, _recvBufferSize, _sendBufferSize)
{
}


