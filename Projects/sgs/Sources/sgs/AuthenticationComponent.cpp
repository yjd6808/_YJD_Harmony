/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 5:29:23 PM
 * =====================
 *
 */


#include "Core.h"
#include "AuthenticationComponent.h"

#include <sgs/UnauthenticatedSessionManager.h>
#include <sgs/_Net/NetSession.h>

USING_NS_SG;

//////////////////////////////////////////////////////////////////////////////////////////
AuthenticationComponent::AuthenticationComponent(sg::NetSession* _pSession)
: SessionComponent(_pSession)
{
	AuthenticationComponent::Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthenticationComponent::Initialize()
{
	state_ = AuthenticationState::Initialized;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthenticationComponent::OnConnected()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthenticationComponent::OnDisconnected()
{
	switch (state_)
	{
	case AuthenticationState::Initialized:
		break;
	case AuthenticationState::LobbyWait:
		RemoveUnauthenticatedSession();
		break;
	case AuthenticationState::Lobby:
		break;
	default:
		jc_assert(false);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthenticationComponent::SetState(AuthenticationState_t _state)
{
	state_ = _state;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthenticationComponent::SetSerial(AuthenticationSerial_t _serial)
{
	serial_ = _serial;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthenticationComponent::SetAccountId(const char* _pAccountId)
{
	accountId_.SetString(_pAccountId);
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthenticationComponent::RemoveUnauthenticatedSession()
{
	if (!g_cUnauthenticatedSessionManager.Remove(serial_))
	{
		_LogWarn_("미인증 세션목록에 %d 시리얼에 해당하는 세션이 없습니다.", serial_);
	}
}
