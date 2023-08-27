/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 6:19:51 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "AuthenticationComponent.h"

AuthenticationComponent::AuthenticationComponent() {
	AuthenticationComponent::initialize();
}

AuthenticationComponent::~AuthenticationComponent() {}


void AuthenticationComponent::initialize() {
	m_AccountData.Clear();

	m_eState = AuthenticationState::Initialized;
	m_iSerial = InvalidValue_v;
}

void AuthenticationComponent::setAccountIdPass(const char* id, const char* pass) {
	m_AccountData.Id.SetString(id);
	m_AccountData.Pass.SetString(pass);
}

void AuthenticationComponent::setLastServer(GameServerType_t lastServer) {
	m_AccountData.LastServer = lastServer;
}

void AuthenticationComponent::setState(AuthenticationState_t state) {
	m_eState = state;
}

void AuthenticationComponent::setSerial(AuthenticationSerial_t serial) {
	m_iSerial = serial;
}
