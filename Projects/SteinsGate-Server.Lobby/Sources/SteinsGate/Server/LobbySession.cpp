/*
 * 작성자: 윤정도
 * 작성일: 8/3/2023 8:34:29 AM
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

//////////////////////////////////////////////////////////////////////////////////////////
LobbySession::LobbySession(TcpServer* _pServer,
	const IOCPPtr& _pIocp,
	const JCore::MemoryPoolAbstractPtr& _pBufferAllocator,
	int _recvBufferSize,
	int _sendBufferSize)
	: CommonSession(_pServer, _pIocp, _pBufferAllocator, _recvBufferSize, _sendBufferSize)
	, authenticationComponent_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void LobbySession::OnCreated()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void LobbySession::OnConnected()
{
	AddComponent(authenticationComponent_ = dbg_new AuthenticationComponent(this));
	components_.OnConnected();
	authenticationComponent_->AddRef();
}

//////////////////////////////////////////////////////////////////////////////////////////
void LobbySession::OnDisconnected()
{
	components_.OnDisconnected();
	components_.Clear();

	JCORE_RELEASE_SAFE(authenticationComponent_);
}

//////////////////////////////////////////////////////////////////////////////////////////
AuthenticationComponent* LobbySession::GetAuthenticationComponent(bool _addRef)
{
	if (authenticationComponent_ == nullptr)
	{
		DebugAssert(false);
		return nullptr;
	}

	if (_addRef)
	{
		authenticationComponent_->AddRef();
	}

	return authenticationComponent_;
}


