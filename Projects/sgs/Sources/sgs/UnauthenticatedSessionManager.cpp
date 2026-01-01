/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 8:31:56 PM
 * =====================
 *
 */


#include "Core.h"
#include "ServerCoreHeader.h"
#include "UnauthenticatedSessionManager.h"

bool UnauthenticatedSessionManager::Add(AuthenticationSerial_t _serial, CommonSession* _pSession)
{
	JCORE_LOCK_GUARD(sync_);
	return sessionMap_.Insert(_serial, _pSession);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UnauthenticatedSessionManager::Remove(AuthenticationSerial_t _serial)
{
	JCORE_LOCK_GUARD(sync_);
	return sessionMap_.Remove(_serial);
}
