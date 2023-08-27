/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 8:31:56 PM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "UnauthenticatedSessionManager.h"

bool UnauthenticatedSessionManager::Add(AuthenticationSerial_t serial, CommonSession* session) {
	JCORE_LOCK_GUARD(m_Sync);
	return m_hSessionMap.Insert(serial, session);
}

bool UnauthenticatedSessionManager::Remove(AuthenticationSerial_t serial) {
	JCORE_LOCK_GUARD(m_Sync);
	return m_hSessionMap.Remove(serial);
}
