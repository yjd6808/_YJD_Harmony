/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/CommonServer.h>

class AreaServer final : public CommonServer
{
public:
	AreaServer(const JNetwork::IOCPPtr& iocp, const JCore::MemoryPoolAbstractPtr& bufferAllocator);
	~AreaServer() noexcept override;

	SGTcpSession* CreateSession() override;
	ServerType_t GetServerType() override { return ServerType::Area; }
protected:
	void OnUpdate(const JCore::TimeSpan& elapsed) override;
};