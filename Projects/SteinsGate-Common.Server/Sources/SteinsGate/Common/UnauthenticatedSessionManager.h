/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 8:27:52 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ServerType.h>
#include <SteinsGate/Common/ICreatable.h>

class UnauthenticatedSessionManager : public ICreatable<UnauthenticatedSessionManager>
{
public:
	bool Add(AuthenticationSerial_t _serial, CommonSession* _pSession);
	bool Remove(AuthenticationSerial_t _serial);

private:
	SGNormalLock sync_;
	SGHashMap<AuthenticationSerial_t, CommonSession*> sessionMap_;
};
