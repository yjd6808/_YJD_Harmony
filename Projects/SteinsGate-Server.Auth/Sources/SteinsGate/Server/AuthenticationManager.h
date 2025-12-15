/*
 * 작성자: 윤정도
 * 생성일: 6/14/2023 11:22:13 PM
 * =====================
 * 역할
 *  - 유저 접속 여부
 *	- 올바른 유저인지 판단
 *
 * 5분동안 최신화되지 않는 토큰들은 제거하도록 한다.
 *
 */

#pragma once

#include <JCore/Pattern/Singleton.h>
#include <JCore/Threading/Scheduler.h>

#include <JCore/Container/TreeMap.h>
#include <JCore/Container/HashMap.h>

#include <SteinsGate/Common/Struct.h>

class AuthenticationManager : public JCore::SingletonPointer<AuthenticationManager>
{
	using SerialDataMap = JCore::HashMap<AuthenticationSerial_t, AuthenticationData*>;
	using AccountIdDataMap = JCore::HashMap<JCore::String, AuthenticationData*>;
	using DateTimeDataMap = JCore::TreeMap<JCore::DateTime, AuthenticationData*>;

	friend class TSingleton;
	AuthenticationManager() = default;
	~AuthenticationManager() = default;

public:
	AuthenticationData* Issue(const JCORE_REF_IN AccountData& _accountData);								// 발급
	bool Exist(AuthenticationSerial_t _serial, const char* _pAccountId);							// 발급되었는지 확인
	AuthenticationData* Update(AuthenticationSerial_t _serial, const char* _pAccountId, AuthenticationState_t _state);	// 최신화시각 갱신 및 상태 업데이트
	bool Remove(AuthenticationSerial_t _serial, const char* _pAccountId);

	void Clear();

	void OnScheduled(JCore::SchedulerTask* _pTask); // 스케줄링 될때마다 수행할 작업

private:
	AuthenticationData* IssueRaw(const JCORE_REF_IN AccountData& _accountData);
	AuthenticationData* FindRaw(const JCore::DateTime& _timeId);
	AuthenticationData* FindRaw(AuthenticationSerial_t _serial);
	AuthenticationData* FindRaw(AuthenticationSerial_t _serial, const char* _pAccountId);
	AuthenticationData* UpdateRaw(AuthenticationSerial_t _serial, const char* _pAccountId, AuthenticationState_t _nextState);
	bool RemoveRaw(const JCore::DateTime& _timeId);
	bool RemoveRaw(AuthenticationSerial_t _serial);
	bool RemoveRaw(AuthenticationSerial_t _serial, const char* _pAccountId);

	bool GenerateSerial(JCORE_OUT AuthenticationSerial_t& _serial);
	bool GenerateTimeId(JCORE_OUT JCore::DateTime& _timeId, AuthenticationState_t _state);

private:
	mutable JCore::NormalLock lock_;

	SerialDataMap serialDataMap_;		// 발급된 시리얼과 토큰 데이터를 묶음
	DateTimeDataMap timeDataMap_;		// 발급된 시각과 토큰 데이터를 묶음 (일정 주기마다 빠르게 토큰 만료처리를 하기위함)
	AccountIdDataMap accountIdDataMap_;	// 어떤 아이디들에게 토큰이 발급되었는지
};
