/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Lobby.h"
#include "LobbyCoreHeader.h"
#include "LobbySession.h"

#include <SteinsGate/Common/AuthenticationComponent.h>

#include <SteinsGate/Common/UnauthenticatedSessionManager.h>

USING_NS_JC;
USING_NS_JNET;

LobbySession::LobbySession(TcpServer* server,
	const IOCPPtr& iocp,
	const JCore::MemoryPoolAbstractPtr& bufferAllocator,
	int recvBufferSize,
	int sendBufferSize)
	: CommonSession(server, iocp, bufferAllocator, recvBufferSize, sendBufferSize)
	, m_pAuthenticationComponent(nullptr)
{}

void LobbySession::OnCreated() {
	AddComponent(m_pAuthenticationComponent = dbg_new AuthenticationComponent(this));
}

void LobbySession::OnConnected() {
	CommonSession::OnConnected();
}

void LobbySession::OnDisconnected() {
	CommonSession::OnDisconnected();
}


