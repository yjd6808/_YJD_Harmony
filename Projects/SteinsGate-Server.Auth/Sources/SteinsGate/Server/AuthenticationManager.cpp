/*
 * 작성자: 윤정도
 * 생성일: 6/14/2023 11:55:59 PM
 * =====================
 *
 */


#include "Auth.h"
#include "AuthCoreHeader.h"
#include "AuthenticationManager.h"

#include <JCore/Random.h>

#define TIMEID_GENERATION_RETRY_COUNT		10
#define SERIAL_GENERATION_RETRY_COUNT		10

USING_NS_JC;

AuthenticationData* AuthenticationManager::Issue(const char* accountId) {
	JCORE_LOCK_GUARD(m_Lock);
	return IssueRaw(accountId);
}

bool AuthenticationManager::Exist(AuthenticationSerial_t serial, const char* accountId) {
	JCORE_LOCK_GUARD(m_Lock);
	return FindRaw(serial, accountId);
}

bool AuthenticationManager::Update(AuthenticationSerial_t serial, const char* accountId, AuthenticationState_t state) {
	JCORE_LOCK_GUARD(m_Lock);
	return UpdateRaw(serial, accountId, state);
}

void AuthenticationManager::Clear() {
	JCORE_LOCK_GUARD(m_Lock);
	m_hmSerialMap.ForEachValueDelete();
	m_hmSerialMap.Clear();
	m_tmTimeMap.Clear();
	m_hmAccountIdMap.Clear();
}

bool AuthenticationManager::Remove(AuthenticationSerial_t serial, const char* accountId) {
	JCORE_LOCK_GUARD(m_Lock);
	return Remove(serial, accountId);
}

void AuthenticationManager::OnScheduled(SchedulerTask* task) {
	static Vector<AuthenticationData*> s_vExpired;
	

	// 만료된 데이터 게더링
	const DateTime now = DateTime::Now();
	{
		JCORE_LOCK_GUARD(m_Lock);
		auto it = m_tmTimeMap.Begin();
		while (it->HasNext()) {
			DateTime$DataMap::TKeyValuePair& cur = it->Current();

			if (cur.Key <= now) {
				s_vExpired.PushBack(cur.Value);
			} else {
				break;
			}
			it->Next();
		}

		for (int i = 0; i < s_vExpired.Size(); ++i) {
			AuthenticationData* pExpiredData = s_vExpired[i];
			RemoveRaw(pExpiredData->Serial);
		}
	}

	// TODO: 만료된 경우 알려줌
	for (int i = 0; i < s_vExpired.Size(); ++i) {
		AuthenticationData* pExpiredData = s_vExpired[i];
		_LogDebug_("%s 인증 데이터 만료 (상태:%s)", pExpiredData->AccountId.Source, AuthenticationState::Name[pExpiredData->State]);
		delete pExpiredData;
	}
	s_vExpired.Clear();
}

AuthenticationData* AuthenticationManager::IssueRaw(const char* accountId) {
	if (m_hmAccountIdMap.Exist(accountId)) {
		return nullptr;
	}

	DateTime timeId;
	AuthenticationSerial_t iSerial;

	if (!GenerateSerial(iSerial)) {
		_LogDebug_("시리얼 생성 실패");
		return nullptr;
	}

	if (!GenerateTimeId(timeId, AuthenticationState::LobbyWait)) {
		_LogDebug_("타임ID 생성 실패 %d", 1);
		return nullptr;
	}

	if (AuthenticationData* pExistData = FindRaw(iSerial)) {
		_LogDebug_("이미 해당 시리얼의 유저가 존재함. (%d:%s)", pExistData->Serial, pExistData->AccountId.Source);
		return nullptr;
	}

	AuthenticationData* pToken = dbg_new AuthenticationData{};
	bool bAdded = true;

	pToken->AccountId = accountId;
	pToken->Serial = iSerial;
	pToken->TimeId = timeId;

	bAdded = m_tmTimeMap.Insert(timeId, pToken);
	if (!bAdded) { DebugAssert(false); return nullptr; }

	bAdded = m_hmSerialMap.Insert(iSerial, pToken);
	if (!bAdded) { DebugAssert(false); return nullptr; }

	bAdded = m_hmAccountIdMap.Insert(pToken->AccountId.Source, pToken);
	if (!bAdded) { DebugAssert(false); return nullptr; }

	return pToken;
}

AuthenticationData* AuthenticationManager::FindRaw(const DateTime& timeId) {
	AuthenticationData** ppData = m_tmTimeMap.Find(timeId);

	if (ppData == nullptr) {
		return nullptr;
	}

	return *ppData;
}

AuthenticationData* AuthenticationManager::FindRaw(AuthenticationSerial_t serial) {
	AuthenticationData** ppFind = m_hmSerialMap.Find(serial);
	if (ppFind == nullptr) {
		return nullptr;
	}
	return *ppFind;
}


AuthenticationData* AuthenticationManager::FindRaw(AuthenticationSerial_t serial, const char* accountId) {
	AuthenticationData** ppFind = m_hmSerialMap.Find(serial);
	if (ppFind == nullptr) {
		return nullptr;
	}

	if ((*ppFind)->AccountId == accountId) {
		return *ppFind;
	}
	return nullptr;
}

bool AuthenticationManager::UpdateRaw(AuthenticationSerial_t serial, const char* accountId, AuthenticationState_t nextState) {

 	AuthenticationData* pData = FindRaw(serial);

	if (pData == nullptr) {
		return false;
	}

	if (pData->AccountId != accountId) {
		return false;
	}

	if (RemoveRaw(pData->TimeId)) {
		_LogDebug_("기존 타임ID 제거 실패");
		return false;
	}

	DateTime timeId;
	bool bAdded = true;

	if (!GenerateTimeId(timeId, nextState)) {
		_LogDebug_("타임ID 생성 실패 %d", 2);
	}

	pData->TimeId = timeId;
	pData->State = nextState;

	bAdded = m_tmTimeMap.Insert(timeId, pData);
	if (!bAdded) { DebugAssert(false); return false; }

	return true;
}

bool AuthenticationManager::RemoveRaw(const DateTime& timeId) {
	return m_tmTimeMap.Remove(timeId);
}

bool AuthenticationManager::RemoveRaw(AuthenticationSerial_t serial) {
	AuthenticationData* pData = FindRaw(serial);

	if (pData == nullptr) {
		return false;
	}

	bool bRemoved = true;
	bRemoved = m_tmTimeMap.Remove(pData->TimeId);
	if (!bRemoved) { DebugAssert(false); return false; }

	bRemoved = m_hmSerialMap.Remove(serial);
	if (!bRemoved) { DebugAssert(false); return false; }

	bRemoved = m_hmAccountIdMap.Remove(pData->AccountId.Source);
	if (!bRemoved) { DebugAssert(false); return false; }
	return true;
}


bool AuthenticationManager::RemoveRaw(AuthenticationSerial_t serial, const char* accountId) {
	AuthenticationData* pData = FindRaw(serial);

	if (pData == nullptr) {
		return false;
	}

	if (pData->AccountId != accountId) {
		return false;
	}

	bool bRemoved = true;
	bRemoved = m_tmTimeMap.Remove(pData->TimeId);
	if (!bRemoved) { DebugAssert(false); return false; }

	bRemoved = m_hmSerialMap.Remove(serial);
	if (!bRemoved) { DebugAssert(false); return false; }

	bRemoved = m_hmAccountIdMap.Remove(accountId);
	if (!bRemoved) { DebugAssert(false); return false; }
	return true;
}


bool AuthenticationManager::GenerateSerial(JCORE_OUT AuthenticationSerial_t& data) {
	AuthenticationSerial_t iGeneratedSerial;
	int iRetry = 0;
	do {
		iGeneratedSerial = Random::GenerateInt(100, 10000000);
		
		if (!FindRaw(iGeneratedSerial)) {
			break;
		}
		++iRetry;
	} while (iRetry < SERIAL_GENERATION_RETRY_COUNT);

	if (iRetry >= SERIAL_GENERATION_RETRY_COUNT) {
		return false;
	}

	data = iGeneratedSerial;
	return true;
}

bool AuthenticationManager::GenerateTimeId(JCORE_OUT DateTime& timeId, AuthenticationState_t state) {
	DateTime generatedTime = DateTime::Now();
	int iRetry = 0;

	switch (state) {
	case AuthenticationState::LobbyWait: generatedTime.AddMiliSecond(1000 * 15);	break;
	case AuthenticationState::Lobby:	 generatedTime.AddMiliSecond(1000 * 900);	break;
	case AuthenticationState::GameWait:  generatedTime.AddMiliSecond(1000 * 15);	break;
	case AuthenticationState::Game:		 generatedTime.AddMiliSecond(1000 * 1800);	break;
	default: _LogWarn_("GenerateTimeId() 실패"); return false;
	}

	do {
		if (!FindRaw(generatedTime)) {
			break;
		}
		generatedTime.Tick++;
		++iRetry;
	} while (iRetry < TIMEID_GENERATION_RETRY_COUNT);

	if (iRetry >= TIMEID_GENERATION_RETRY_COUNT) {
		return false;
	}

	timeId = generatedTime;
	return true;
}


