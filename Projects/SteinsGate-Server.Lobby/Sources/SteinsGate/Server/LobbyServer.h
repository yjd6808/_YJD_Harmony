/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/CommonServer.h>

class LobbyServer final : public CommonServer
{
public:
	LobbyServer(const JNetwork::IOCPPtr& _pIocp, const JCore::MemoryPoolAbstractPtr& _pBufferAllocator);

	SGTcpSession* CreateSession() override;
	ServerType_t GetServerType() override { return ServerType::Lobby; }
protected:
	void OnUpdate(const JCore::TimeSpan& _elapsed) override;
};