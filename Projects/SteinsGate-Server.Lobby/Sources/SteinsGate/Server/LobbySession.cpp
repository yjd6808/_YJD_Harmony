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

}

void LobbySession::OnConnected() {
	AddComponent(m_pAuthenticationComponent = dbg_new AuthenticationComponent(this));
	m_Components.OnConnected();
	m_pAuthenticationComponent->AddRef();
}

void LobbySession::OnDisconnected() {
	m_Components.OnDisconnected();
	m_Components.Clear();

	JCORE_RELEASE_SAFE(m_pAuthenticationComponent);
}

AuthenticationComponent* LobbySession::GetAuthenticationComponent(bool addRef) {
	if (m_pAuthenticationComponent == nullptr) {
		DebugAssert(false);
		return nullptr;
	}

	if (addRef) {
		m_pAuthenticationComponent->AddRef();
	}

	return m_pAuthenticationComponent; 
}


