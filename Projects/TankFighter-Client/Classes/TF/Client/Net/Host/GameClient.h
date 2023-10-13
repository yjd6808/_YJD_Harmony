/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 12:30:33 PM
 * =====================
 *
 */


#pragma once


#include <JNetwork/Host/TcpClient.h>

class GameClient : public JNetwork::TcpClient
{
public:
	GameClient(const JNetwork::IOCPPtr& iocp, const JCore::MemoryPoolAbstractPtr& bufferAllocator);
};
