/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 6:04:54 PM
 * =====================
 *
 */


#pragma once

#include "sgcl/IComponent.h"

class AuthenticationComponent : public IComponent
{
public:
	AuthenticationComponent();
	~AuthenticationComponent() override;

	void Initialize() override;
	Type GetType() const override { return eAuthentication; }

	void SetAccountIdPass(const char* _pId, const char* _pPass);
	void SetLastServer(GameServerType_t _lastServer);
	void SetState(AuthenticationState_t _state);
	void SetSerial(AuthenticationSerial_t _serial);

	AuthenticationSerial_t GetSerial() const { return serial_; }
	AuthenticationState_t GetState() const { return state_; }
	const AccountData& GetAccountData() const { return accountData_; }

private:
	AccountData accountData_;
	AuthenticationSerial_t serial_;
	AuthenticationState_t state_;
};
