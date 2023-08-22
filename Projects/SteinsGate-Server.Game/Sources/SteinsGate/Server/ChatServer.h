/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/CommonServer.h>

class ChatServer final : public CommonServer
{
public:
	ChatServer(const JNetwork::IOCPPtr& iocp, const JCore::MemoryPoolAbstractPtr& bufferAllocator);

	SGTcpSession* CreateSession() override;
	ServerType_t GetServerType() override { return ServerType::Chat; }
protected:
	void OnUpdate(const JCore::TimeSpan& elapsed) override;
};