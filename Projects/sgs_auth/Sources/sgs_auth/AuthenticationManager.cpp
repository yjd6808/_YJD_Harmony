/*
 * 작성자: 윤정도
 * 생성일: 6/14/2023 11:55:59 PM
 * =====================
 *
 */

#include "Core.h"
#include "AuthenticationManager.h"

#include <jc/Random.h>

#define TIMEID_GENERATION_RETRY_COUNT        10
#define SERIAL_GENERATION_RETRY_COUNT        10

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
AuthenticationData* AuthenticationManager::Issue(const  AccountData& _accountData)
{
	JC_LOCK_GUARD(lock_);
	return IssueRaw(_accountData);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool AuthenticationManager::Exist(AuthenticationSerial_t _serial, const char* _pAccountId)
{
	JC_LOCK_GUARD(lock_);
	return FindRaw(_serial, _pAccountId) != nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
AuthenticationData* AuthenticationManager::Update(AuthenticationSerial_t _serial, const char* _pAccountId, AuthenticationState_t _state)
{
	JC_LOCK_GUARD(lock_);
	return UpdateRaw(_serial, _pAccountId, _state);
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthenticationManager::Clear()
{
	JC_LOCK_GUARD(lock_);
	serialDataMap_.ForEachValueDelete();
	serialDataMap_.Clear();
	timeDataMap_.Clear();
	accountIdDataMap_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool AuthenticationManager::Remove(AuthenticationSerial_t _serial, const char* _pAccountId)
{
	JC_LOCK_GUARD(lock_);
	return RemoveRaw(_serial, _pAccountId);
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthenticationManager::OnScheduled(SchedulerTask* _pTask)
{
	Vector<AuthenticationData*> expiredList;

	const DateTime now = DateTime::Now();
	{
		JC_LOCK_GUARD(lock_);
		auto it = timeDataMap_.Begin();

		while (it->HasNext())
		{
			DateTimeDataMap::TKeyValuePair& current = it->Current();

			if (current.key_ <= now)
			{
				expiredList.PushBack(current.value_);
			}
			else
			{
				break;
			}

			it->Next();
		}

		for (int i = 0; i < expiredList.Size(); ++i)
		{
			AuthenticationData* pExpiredData = expiredList[i];
			RemoveRaw(pExpiredData->serial_);
		}
	}

	// TODO: 만료된 경우 알려줌
	for (int i = 0; i < expiredList.Size(); ++i)
	{
		AuthenticationData* pExpiredData = expiredList[i];
		_LogDebug_("%s 인증 데이터 만료 (상태:%s)", pExpiredData->accountData_.id_.Source, AuthenticationState::Name[pExpiredData->state_]);
		delete pExpiredData;
	}

	expiredList.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
AuthenticationData* AuthenticationManager::IssueRaw(const  AccountData& _accountData)
{
	if (accountIdDataMap_.Exist(_accountData.id_.Source))
	{
		return nullptr;
	}

	DateTime timeId;
	AuthenticationSerial_t serial;

	if (!GenerateSerial(serial))
	{
		_LogDebug_("시리얼 생성 실패");
		return nullptr;
	}

	if (!GenerateTimeId(timeId, AuthenticationState::LobbyWait))
	{
		_LogDebug_("타임ID 생성 실패 %d", 1);
		return nullptr;
	}

	if (AuthenticationData* pExistData = FindRaw(serial))
	{
		_LogDebug_("이미 해당 시리얼의 유저가 존재함. (%d:%s)", pExistData->serial_, pExistData->accountData_.id_.Source);
		return nullptr;
	}

	AuthenticationData* pToken = dbg_new AuthenticationData{};
	bool added = true;

	pToken->accountData_ = _accountData;
	pToken->serial_ = serial;
	pToken->timeId_ = timeId;

	added = timeDataMap_.Insert(timeId, pToken);
	if (!added)
	{
		jc_assert(false);
		return nullptr;
	}

	added = serialDataMap_.Insert(serial, pToken);
	if (!added)
	{
		jc_assert(false);
		return nullptr;
	}

	added = accountIdDataMap_.Insert(pToken->accountData_.id_.Source, pToken);
	if (!added)
	{
		jc_assert(false);
		return nullptr;
	}

	return pToken;
}

//////////////////////////////////////////////////////////////////////////////////////////
AuthenticationData* AuthenticationManager::FindRaw(const DateTime& _timeId)
{
	AuthenticationData** pDataPtr = timeDataMap_.Find(_timeId);

	if (pDataPtr == nullptr)
	{
		return nullptr;
	}

	return *pDataPtr;
}

//////////////////////////////////////////////////////////////////////////////////////////
AuthenticationData* AuthenticationManager::FindRaw(AuthenticationSerial_t _serial)
{
	AuthenticationData** pFindPtr = serialDataMap_.Find(_serial);

	if (pFindPtr == nullptr)
	{
		return nullptr;
	}

	return *pFindPtr;
}

//////////////////////////////////////////////////////////////////////////////////////////
AuthenticationData* AuthenticationManager::FindRaw(AuthenticationSerial_t _serial, const char* _pAccountId)
{
	AuthenticationData** pFindPtr = serialDataMap_.Find(_serial);

	if (pFindPtr == nullptr)
	{
		return nullptr;
	}

	if ((*pFindPtr)->accountData_.id_ == _pAccountId)
	{
		return *pFindPtr;
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
AuthenticationData* AuthenticationManager::UpdateRaw(AuthenticationSerial_t _serial, const char* _pAccountId, AuthenticationState_t _nextState)
{
	AuthenticationData* pData = FindRaw(_serial);

	if (pData == nullptr)
	{
		return nullptr;
	}

	if (pData->accountData_.id_ != _pAccountId)
	{
		_LogWarn_("시리얼은 동일하지만 ID가 다른 유저입니다.");
		return nullptr;
	}

	if (pData->state_ == _nextState)
	{
		jc_assert_msg(false, "동일한 인증상태로 업데이트를 시도했습니다.");
		return nullptr;
	}

	if (!RemoveRaw(pData->timeId_))
	{
		_LogDebug_("기존 타임ID 제거 실패");
		return nullptr;
	}

	DateTime timeId;
	bool added = true;

	if (!GenerateTimeId(timeId, _nextState))
	{
		_LogDebug_("타임ID 생성 실패 %d", 2);
	}

	pData->timeId_ = timeId;
	pData->state_ = _nextState;

	added = timeDataMap_.Insert(timeId, pData);
	if (!added)
	{
		jc_assert(false);
		return nullptr;
	}

	return pData;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool AuthenticationManager::RemoveRaw(const DateTime& _timeId)
{
	return timeDataMap_.Remove(_timeId);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool AuthenticationManager::RemoveRaw(AuthenticationSerial_t _serial)
{
	AuthenticationData* pData = FindRaw(_serial);

	if (pData == nullptr)
	{
		return false;
	}

	bool removed = true;

	removed = timeDataMap_.Remove(pData->timeId_);
	if (!removed)
	{
		jc_assert(false);
		return false;
	}

	removed = serialDataMap_.Remove(_serial);
	if (!removed)
	{
		jc_assert(false);
		return false;
	}

	removed = accountIdDataMap_.Remove(pData->accountData_.id_.Source);
	if (!removed)
	{
		jc_assert(false);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool AuthenticationManager::RemoveRaw(AuthenticationSerial_t _serial, const char* _pAccountId)
{
	AuthenticationData* pData = FindRaw(_serial);

	if (pData == nullptr)
	{
		return false;
	}

	if (pData->accountData_.id_.Source != _pAccountId)
	{
		return false;
	}

	bool removed = true;

	removed = timeDataMap_.Remove(pData->timeId_);
	if (!removed)
	{
		jc_assert(false);
		return false;
	}

	removed = serialDataMap_.Remove(_serial);
	if (!removed)
	{
		jc_assert(false);
		return false;
	}

	removed = accountIdDataMap_.Remove(_pAccountId);
	if (!removed)
	{
		jc_assert(false);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool AuthenticationManager::GenerateSerial(OUT AuthenticationSerial_t& _serial)
{
	AuthenticationSerial_t generatedSerial;
	int retryCount = 0;

	do
	{
		generatedSerial = g_cRandom.GenerateInt(Const::Authentication::SerialRange.Min, Const::Authentication::SerialRange.Max);

		if (!FindRaw(generatedSerial))
		{
			break;
		}

		++retryCount;
	}
	while (retryCount < SERIAL_GENERATION_RETRY_COUNT);

	if (retryCount >= SERIAL_GENERATION_RETRY_COUNT)
	{
		return false;
	}

	_serial = generatedSerial;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool AuthenticationManager::GenerateTimeId(OUT DateTime& _timeId, AuthenticationState_t _state)
{
	DateTime generatedTime = DateTime::Now();
	int retryCount = 0;

	switch (_state)
	{
	case AuthenticationState::LobbyWait:
		generatedTime.AddMiliSecond(1000 * Const::Timeout::Authentication::LobbyWait);
		break;
	case AuthenticationState::Lobby:
		generatedTime.AddMiliSecond(1000 * Const::Timeout::Authentication::Lobby);
		break;
	case AuthenticationState::GameWait:
		generatedTime.AddMiliSecond(1000 * Const::Timeout::Authentication::GameWait);
		break;
	case AuthenticationState::Game:
		generatedTime.AddMiliSecond(1000 * Const::Timeout::Authentication::Game);
		break;
	default:
		_LogWarn_("GenerateTimeId() 실패");
		return false;
	}

	do
	{
		if (!FindRaw(generatedTime))
		{
			break;
		}

		generatedTime.Tick++;
		++retryCount;
	}
	while (retryCount < TIMEID_GENERATION_RETRY_COUNT);

	if (retryCount >= TIMEID_GENERATION_RETRY_COUNT)
	{
		return false;
	}

	_timeId = generatedTime;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AuthenticationManager::Schedule::OnScheduled()
{
	if (!AuthenticationManager::Singleton_IsDeleted()) // TODO: 싱글톤 삭제될 때 호출되는 케이스는 애초에 없어야한다. 매니저 삭제전에 스케쥴러가 먼저 정지가 되어야함,
	{
		AuthenticationManager::Get()->OnScheduled(this);
	}
}
