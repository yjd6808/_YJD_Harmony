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
	Type getType() const override { return eAuthentication; }

	void setAccountIdPass(const char* _pId, const char* _pPass);
	void setLastServer(GameServerType_t _lastServer);
	void setState(AuthenticationState_t _state);
	void setSerial(AuthenticationSerial_t _serial);

	AuthenticationSerial_t getSerial() const { return serial_; }
	AuthenticationState_t getState() const { return state_; }
	const AccountData& getAccountData() const { return accountData_; }

private:
	AccountData accountData_;
	AuthenticationSerial_t serial_;
	AuthenticationState_t state_;
};
