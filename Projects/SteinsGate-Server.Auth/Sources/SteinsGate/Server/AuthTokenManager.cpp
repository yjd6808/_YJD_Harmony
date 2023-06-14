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
	LOCK_GUARD(m_Lock);
	return IssueRaw(accountTableId, id);
}

bool AuthTokenManager::Check(const String& token, const char* id) {
	LOCK_GUARD(m_Lock);
	return CheckRaw(token, id);
}

bool AuthTokenManager::Check(int accountTableId, const char* id) {
	LOCK_GUARD(m_Lock);
	return CheckRaw(accountTableId, id);
}

bool AuthTokenManager::Update(const String& token) {
	LOCK_GUARD(m_Lock);
	return UpdateRaw(token);
}

bool AuthTokenManager::Update(int accountTableId) {
	LOCK_GUARD(m_Lock);
	return UpdateRaw(accountTableId);
}

void AuthTokenManager::Clear() {
	LOCK_GUARD(m_Lock);
	m_tmTokenMap.ForEachValue([](AuthToken* token) { delete token; });
	m_tmTokenMap.Clear();
	m_tmAccountTableIdMap.Clear();
}

bool AuthTokenManager::Remove(const String& token) {
	LOCK_GUARD(m_Lock);
	return RemoveRaw(token);
}

bool AuthTokenManager::Remove(int accountTableId) {
	LOCK_GUARD(m_Lock);
	return RemoveRaw(accountTableId);
}

void AuthTokenManager::OnScheduled(SchedulerTask* task) {
	LOCK_GUARD(m_Lock);

	// TODO: 
}

bool AuthTokenManager::IssueRaw(int accountTableId, const char* id) {
	AuthToken* pToken = dbg_new AuthToken{};

	if (CheckRaw(accountTableId, id)) {
		return false;
	}

	GenerateTokenData(pToken->Data);
	pToken->Id = id;
	pToken->AccountTableId = accountTableId;
	pToken->Updated = DateTime::Now();

	m_tmAccountTableIdMap.Insert(accountTableId, pToken);
	m_tmTokenMap.Insert(pToken->Data.Source, pToken);
	return true;
}

bool AuthTokenManager::CheckRaw(const String& token) {
	return m_tmTokenMap.Exist(token);
}

bool AuthTokenManager::CheckRaw(int accountTableId) {
	return m_tmAccountTableIdMap.Exist(accountTableId);
}


bool AuthTokenManager::CheckRaw(const String& token, const char* id) {
	if (!m_tmTokenMap.Exist(token)) {
		return false;
	}

	const AuthToken* pToken = m_tmTokenMap[token];
	if (pToken->Id != id) {
		RemoveRaw(token);		// 토큰은 동일한데, 아디 비번이 틀리면 강제로 제거
		return false;
	}

	return true;
}

bool AuthTokenManager::CheckRaw(int accountTableId, const char* id) {
	if (!m_tmAccountTableIdMap.Exist(accountTableId)) {
		return false;
	}

	const AuthToken* pToken = m_tmAccountTableIdMap[accountTableId];
	if (pToken->Id != id) {
		RemoveRaw(pToken->AccountTableId);	// 테이블 ID가 동일한데, 아디 비번이 틀리면 강제 제거
		return false;
	}
	return true;
}

bool AuthTokenManager::UpdateRaw(const String& token) {

	if (!CheckRaw(token)) {
		return false;
	}

	m_tmTokenMap[token]->Updated = DateTime::Now();
	return true;
}

bool AuthTokenManager::UpdateRaw(int accountTableId) {
	if (!CheckRaw(accountTableId)) {
		return false;
	}

	m_tmAccountTableIdMap[accountTableId]->Updated = DateTime::Now();
	return true;
}

bool AuthTokenManager::RemoveRaw(const JCore::String& token) {
	AuthToken* pToken;
	AuthToken** ppFind = m_tmTokenMap.Find(token);
	if (ppFind == nullptr) {
		return false;
	}
	pToken = *ppFind;
	m_tmTokenMap.Remove(token);
	m_tmAccountTableIdMap.Remove(pToken->AccountTableId);
	delete pToken;
	return true;
}

bool AuthTokenManager::RemoveRaw(int accountTableId) {
	AuthToken* pToken;
	AuthToken** ppFind = m_tmAccountTableIdMap.Find(accountTableId);
	if (ppFind == nullptr) {
		return false;
	}
	pToken = *ppFind;
	m_tmTokenMap.Remove(pToken->Data.Source);
	m_tmAccountTableIdMap.Remove(pToken->AccountTableId);
	delete pToken;
	return true;
}

void AuthTokenManager::GenerateTokenData(JCORE_OUT StaticString<AuthTokenLen_v>& token) {
	String szTokenData;
	do {
		for (int i = 0; i < AuthTokenLen_v; ++i) {
			szTokenData += Random::GenerateAlphabat();
		}
		
		if (!CheckRaw(szTokenData)) {
			break;
		}

		szTokenData.Clear(); // 있으면 다시 생성
	} while (true); 

	token = szTokenData;
}


