/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 8:19:11 PM
 * =====================
 *
 */

#include "Pch.h"
#include "GameSession.h"

GameSession::GameSession(
	JNetwork::TcpServer* server, 
	const JNetwork::IOCPPtr& iocp,
	const JCore::MemoryPoolAbstractPtr& bufferAllocator)
	: TcpSession(server, iocp, bufferAllocator, 4096, 4096)
{}
