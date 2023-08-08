/*
 * 작성자: 윤정도
 * 생성일: 6/14/2023 11:55:59 PM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthTokenManager.h"

#include <JCore/Random.h>

USING_NS_JC;

bool AuthTokenManager::Issue(int accountTableId, const char* id) {
	JCORE_LOCK_GUARD(m_Lock);
	return IssueRaw(accountTableId, id);
}

bool AuthTokenManager::CheckBySerial(AuthTokenSerial_t serial, const char* id) {
	JCORE_LOCK_GUARD(m_Lock);
	return CheckRawBySerial(serial, id);
}

bool AuthTokenManager::CheckByTableId(int accountTableId, const char* id) {
	JCORE_LOCK_GUARD(m_Lock);
	return CheckRawByTableId (accountTableId, id);
}

bool AuthTokenManager::UpdateBySerial(AuthTokenSerial_t serial) {
	JCORE_LOCK_GUARD(m_Lock);
	return UpdateRawBySerial(serial);
}

bool AuthTokenManager::UpdateByTableId(int accountTableId) {
	JCORE_LOCK_GUARD(m_Lock);
	return UpdateRawByTableId(accountTableId);
}

void AuthTokenManager::Clear() {
	JCORE_LOCK_GUARD(m_Lock);
	m_tmSerialMap.ForEachValueDelete();
	m_tmSerialMap.Clear();
	m_tmAccountTableIdMap.Clear();
}

bool AuthTokenManager::RemoveBySerial(AuthTokenSerial_t serial) {
	JCORE_LOCK_GUARD(m_Lock);
	return RemoveRawBySerial(serial);
}

bool AuthTokenManager::RemoveByTableId(int accountTableId) {
	JCORE_LOCK_GUARD(m_Lock);
	return RemoveRawByTableId(accountTableId);
}

void AuthTokenManager::OnScheduled(SchedulerTask* task) {
	JCORE_LOCK_GUARD(m_Lock);

	// TODO: 
}

bool AuthTokenManager::IssueRaw(int accountTableId, const char* id) {
	AuthToken* pToken = dbg_new AuthToken{};

	if (CheckRawByTableId(accountTableId, id)) {
		return false;
	}

	GenerateSerial(pToken->Serial);
	pToken->Id = id;
	pToken->AccountDBTableId = accountTableId;
	pToken->Updated = DateTime::Now();

	m_tmAccountTableIdMap.Insert(accountTableId, pToken);
	m_tmSerialMap.Insert(pToken->Serial, pToken);
	return true;
}

bool AuthTokenManager::CheckRawBySerial(AuthTokenSerial_t serial) {
	return m_tmSerialMap.Exist(serial);
}

bool AuthTokenManager::CheckRawByTableId(int accountTableId) {
	return m_tmAccountTableIdMap.Exist(accountTableId);
}


bool AuthTokenManager::CheckRawBySerial(AuthTokenSerial_t serial, const char* id) {
	if (!m_tmSerialMap.Exist(serial)) {
		return false;
	}

	const AuthToken* pToken = m_tmSerialMap[serial];
	if (pToken->Id != id) {
		RemoveRawBySerial(serial);		// 토큰은 동일한데(해킹 의심), 아디 비번이 틀리면 강제로 제거
		return false;
	}

	return true;
}

bool AuthTokenManager::CheckRawByTableId(int accountTableId, const char* id) {
	if (!m_tmAccountTableIdMap.Exist(accountTableId)) {
		return false;
	}

	const AuthToken* pToken = m_tmAccountTableIdMap[accountTableId];
	if (pToken->Id != id) {
		RemoveRawByTableId(pToken->AccountDBTableId);	// 테이블 ID가 동일한데, 아디 비번이 틀리면 강제 제거
		return false;
	}
	return true;
}

bool AuthTokenManager::UpdateRawBySerial(AuthTokenSerial_t serial) {

	if (!CheckRawBySerial(serial)) {
		return false;
	}

	m_tmSerialMap[serial]->Updated = DateTime::Now();
	return true;
}

bool AuthTokenManager::UpdateRawByTableId(int accountTableId) {
	if (!CheckRawByTableId(accountTableId)) {
		return false;
	}

	m_tmAccountTableIdMap[accountTableId]->Updated = DateTime::Now();
	return true;
}

bool AuthTokenManager::RemoveRawBySerial(AuthTokenSerial_t serial) {
	AuthToken* pToken;
	AuthToken** ppFind = m_tmSerialMap.Find(serial);
	if (ppFind == nullptr) {
		return false;
	}
	pToken = *ppFind;
	m_tmSerialMap.Remove(serial);
	m_tmAccountTableIdMap.Remove(pToken->AccountDBTableId);
	delete pToken;
	return true;
}

bool AuthTokenManager::RemoveRawByTableId(int accountTableId) {
	AuthToken* pToken;
	AuthToken** ppFind = m_tmAccountTableIdMap.Find(accountTableId);
	if (ppFind == nullptr) {
		return false;
	}
	pToken = *ppFind;
	m_tmSerialMap.Remove(pToken->Serial);
	m_tmAccountTableIdMap.Remove(pToken->AccountDBTableId);
	delete pToken;
	return true;
}

void AuthTokenManager::GenerateSerial(JCORE_OUT AuthTokenSerial_t& data) {
	AuthTokenSerial_t iGeneratedSerial;
	do {
		iGeneratedSerial = Random::GenerateInt(100, 10000000);
		
		if (!CheckRawBySerial(iGeneratedSerial)) {
			break;
		}

	} while (true); 

	data = iGeneratedSerial;
}


