/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 8:31:56 PM
 * =====================
 *
 */


#include "UnauthenticatedSessionManager.h"

#include "sgs/_Net/NetSession.h"

USING_NS_SG;

bool UnauthenticatedSessionManager::Add(AuthenticationSerial_t _serial, sg::NetSession* _pSession)
{
	JC_LOCK_GUARD(sync_);
	return sessionMap_.Insert(_serial, _pSession);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UnauthenticatedSessionManager::Remove(AuthenticationSerial_t _serial)
{
	JC_LOCK_GUARD(sync_);
	return sessionMap_.Remove(_serial);
}
