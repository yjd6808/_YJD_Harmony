/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/CommonSession.h>

class AuthenticationComponent;
class LobbySession : public CommonSession
{
public:
	LobbySession(
		JNetwork::TcpServer* server,
		const JNetwork::IOCPPtr& iocp,
		const JCore::MemoryPoolAbstractPtr& bufferAllocator,
		int recvBufferSize,
		int sendBufferSize
	);

	void OnCreated() override;
	void OnConnected() override;
	void OnDisconnected() override;

	AuthenticationComponent* GetAuthenticationComponent(bool addRef = false);
private:
	AuthenticationComponent* m_pAuthenticationComponent;
};


