/*
 * 작성자: 윤정도
 * 작성일: 8/3/2023 8:34:29 AM
 * =====================
 *
 */


#pragma once

#include <sgs/CommonSession.h>

class AuthenticationComponent;
class LobbySession : public CommonSession
{
public:
	LobbySession(
		jnet::TcpServer* _pServer,
		const jnet::IOCPPtr& _pIocp,
		const jc::MemoryPoolAbstractPtr& _pBufferAllocator,
		int _recvBufferSize,
		int _sendBufferSize);

	void OnCreated() override;
	void OnConnected() override;
	void OnDisconnected() override;

	AuthenticationComponent* GetAuthenticationComponent(bool _addRef = false);
private:
	AuthenticationComponent* authenticationComponent_;
};


