/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 12:01:18 AM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthSession.h"

USING_NS_JC;
USING_NS_JNET;

AuthSession::AuthSession(
	TcpServer* server,
	const IOCPPtr& iocp,
	const JCore::MemoryPoolAbstractPtr& bufferAllocator,
	int recvBufferSize,
	int sendBufferSize) : TcpSession(server, iocp, bufferAllocator, nullptr, recvBufferSize, sendBufferSize)
{}


