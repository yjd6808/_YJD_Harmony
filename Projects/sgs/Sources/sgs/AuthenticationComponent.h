/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 5:29:13 PM
 * =====================
 *
 */


#pragma once


#include <sgs/SessionComponent.h>
#include <sgs/ISessionEventHandler.h>

class AuthenticationComponent : public SessionComponent
{
public:
	AuthenticationComponent(CommonSession* _pSession);

	void Initialize() override;
	void OnConnected() override;
	void OnDisconnected() override;

	void SetState(AuthenticationState_t _state);
	void SetSerial(AuthenticationSerial_t _serial);
	void SetAccountId(const char* _pAccountId);

	AuthenticationState_t GetState() const { return state_; }
	AuthenticationSerial_t GetSerial() const { return serial_; }

	SG_COMPONENT_TYPE_GETTER(Const::Component::Authentication)

protected:
	void RemoveUnauthenticatedSession();

private:
	jc::StaticString<Const::StringLen::AccountId> accountId_;
	AuthenticationState_t state_;
	AuthenticationSerial_t serial_;
};
