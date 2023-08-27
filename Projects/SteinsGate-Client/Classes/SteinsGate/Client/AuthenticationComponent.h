/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 6:04:54 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/IComponent.h>

class AuthenticationComponent : public IComponent
{
public:
	AuthenticationComponent();
	~AuthenticationComponent() override;

	void initialize() override;
	Type getType() const override { return  eAuthentication; }

	void setAccountIdPass(const char* id, const char* pass);
	void setLastServer(GameServerType_t lastServer);
	void setState(AuthenticationState_t state);

	AuthenticationSerial_t getSerial() const { return m_iSerial; }
	AuthenticationState_t getState() const { return m_eState; }
	const AccountData& getAccountData() const { return m_AccountData; }

private:
	AccountData m_AccountData;

	AuthenticationSerial_t m_iSerial;
	AuthenticationState_t m_eState;
};
