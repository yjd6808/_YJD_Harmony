/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 8:27:52 PM
 * =====================
 *
 */


#pragma once

class UnauthenticatedSessionManager
{
public:
	bool Add(AuthenticationSerial_t _serial, CommonSession* _pSession);
	bool Remove(AuthenticationSerial_t _serial);

private:
	jc::NormalLock sync_;
	jc::HashMap<AuthenticationSerial_t, CommonSession*> sessionMap_;
};
