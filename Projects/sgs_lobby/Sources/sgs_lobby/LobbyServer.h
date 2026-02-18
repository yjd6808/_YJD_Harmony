/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once


#include <sgs/_Net/NetServer.h>

class LobbyServer final : public sg::NetServer
{
public:
	LobbyServer(const jnet::IOCPPtr& _pIocp, const jc::MemoryPoolAbstractPtr& _pBufferAllocator);

	jnet::TcpSession* CreateSession() override;
	ServerType_t GetServerType() override { return ServerType::Lobby; }
protected:
	void OnUpdate(const jc::TimeSpan& _elapsed) override;
};