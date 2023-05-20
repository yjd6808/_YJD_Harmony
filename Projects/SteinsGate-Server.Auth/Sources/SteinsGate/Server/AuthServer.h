/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 3:53:19 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/CommonServer.h>

class AuthServer final : public CommonServer
{
public:
	AuthServer(
		const JNetwork::IOCPPtr& iocp,
		const JCore::MemoryPoolAbstractPtr& bufferAllocator,
		JNetwork::ServerEventListener* eventListener,
		int maxConn,
		int sessionRecvBufferSize,
		int sessionSendBufferSize
	);

	SGTcpSession* CreateSession() override;
	ServerType_t GetServerType() override { return ServerType::Auth; }
	ServerInfo GetServerInfo() override;
protected:
	void OnLoop(JCore::PulserStatistics* pulserStat) override;
};