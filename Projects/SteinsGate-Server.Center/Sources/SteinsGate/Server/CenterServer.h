/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 3:53:19 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Host/TcpServer.h>

#include <SteinsGate/Server/CenterSession.h>

class CenterServer final : public JNetwork::TcpServer
{
public:
	CenterServer(
		const JNetwork::IOCPPtr& iocp,
		const JCore::MemoryPoolAbstractPtr& bufferAllocator,
		JNetwork::TcpServerEventListener* eventListener,
		int maxConn,
		int sessionRecvBufferSize,
		int sessionSendBufferSize
	);

	SGTcpSession* CreateSession() override;
};