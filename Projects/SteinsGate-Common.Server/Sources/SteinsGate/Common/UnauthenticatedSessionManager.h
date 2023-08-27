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
	bool Add(AuthenticationSerial_t serial, CommonSession* session);
	bool Remove(AuthenticationSerial_t serial);
private:
	SGNormalLock m_Sync;
	SGHashMap<AuthenticationSerial_t, CommonSession*> m_hSessionMap;
};
