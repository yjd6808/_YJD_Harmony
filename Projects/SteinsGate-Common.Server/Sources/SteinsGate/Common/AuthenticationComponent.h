/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 5:29:13 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/SessionComponent.h>
#include <SteinsGate/Common/ISessionEventHandler.h>

class AuthenticationComponent : public SessionComponent
{
public:
	AuthenticationComponent(CommonSession* session);

	void Initialize() override;
	void OnConnected() override;
	void OnDisconnected() override;

	void SetState(AuthenticationState_t state);
	void SetSerial(AuthenticationSerial_t serial);

	AuthenticationState_t GetState() const { return m_eState; }
	AuthenticationSerial_t GetSerial() const { return m_iSerial; }

	SG_COMPONENT_TYPE_GETTER(Const::Component::Authentication)

protected:
	void RemoveUnauthenticatedSession();
private:
	AuthenticationState_t m_eState;
	AuthenticationSerial_t m_iSerial;
};