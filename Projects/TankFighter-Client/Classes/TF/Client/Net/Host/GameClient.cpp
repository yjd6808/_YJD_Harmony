/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 12:32:02 PM
 * =====================
 *
 */


#include "Pch.h"
#include "GameClient.h"

GameClient::GameClient(const JNetwork::IOCPPtr& iocp, const JCore::MemoryPoolAbstractPtr& bufferAllocator)
	: TcpClient(iocp, bufferAllocator)
{}
