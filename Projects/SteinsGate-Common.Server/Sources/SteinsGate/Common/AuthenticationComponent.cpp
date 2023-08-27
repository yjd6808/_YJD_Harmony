/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 5:29:23 PM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "AuthenticationComponent.h"

#include "ServerContents.h"
#include "UnauthenticatedSessionManager.h"


AuthenticationComponent::AuthenticationComponent(CommonSession* session)
	: SessionComponent(session)
{
	AuthenticationComponent::Initialize();
}

void AuthenticationComponent::Initialize() {
	m_eState = AuthenticationState::Initialized;
}

void AuthenticationComponent::OnConnected() {
}

void AuthenticationComponent::OnDisconnected() {
	switch (m_eState) {
	case AuthenticationState::Initialized:
		break;
	case AuthenticationState::LobbyWait:
		RemoveUnauthenticatedSession();
		break;
	case AuthenticationState::Lobby:		
		break;
	default: DebugAssert(false);
	}
}

void AuthenticationComponent::SetState(AuthenticationState_t state) {
	m_eState = state;
}

void AuthenticationComponent::SetSerial(AuthenticationSerial_t serial) {
	m_iSerial = serial;
}

void AuthenticationComponent::SetAccountId(const char* accountId) {
	m_szAccountId.SetString(accountId);
}

void AuthenticationComponent::RemoveUnauthenticatedSession() {
	if (!Core::CommonContents->UnauthenticatedSessionManager->Remove(m_iSerial)) {
		_LogWarn_("미인증 세션목록에 %d 시리얼에 해당하는 세션이 없습니다.", m_iSerial);
	}
}
