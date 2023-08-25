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
	using Serial$DataMap = JCore::HashMap<AuthenticationSerial_t, AuthenticationData*>;
	using AccountId$DataMap = JCore::HashMap<JCore::String, AuthenticationData*>;
	using DateTime$DataMap = JCore::TreeMap<JCore::DateTime, AuthenticationData*>;

	friend class TSingleton;
	AuthenticationManager() = default;
	~AuthenticationManager() override = default;
public:
	AuthenticationData* Issue(const char* accountId);																// 발급
	bool Exist(AuthenticationSerial_t serial, const char* accountId);								// 발급되었는지 확인
	bool Update(AuthenticationSerial_t serial, const char* accountId, AuthenticationState_t state);	// 최신화시각 갱신 및 상태 업데이트
	bool Remove(AuthenticationSerial_t serial, const char* accountId);
	void Clear();

	void OnScheduled(JCore::SchedulerTask* task);					// 스케쥴링 될때마다 수행할 작업
private:
	AuthenticationData* IssueRaw(const char* id);
	AuthenticationData* FindRaw(const JCore::DateTime& timeId);
	AuthenticationData* FindRaw(AuthenticationSerial_t serial);
	AuthenticationData* FindRaw(AuthenticationSerial_t serial, const char* accountId);
	bool UpdateRaw(AuthenticationSerial_t serial, const char* accountId, AuthenticationState_t nextState);
	bool RemoveRaw(const JCore::DateTime& timeId);
	bool RemoveRaw(AuthenticationSerial_t serial);
	bool RemoveRaw(AuthenticationSerial_t serial, const char* accountId);

	bool GenerateSerial(JCORE_OUT AuthenticationSerial_t& token);
	bool GenerateTimeId(JCORE_OUT JCore::DateTime& timeId, AuthenticationState_t state);
private:
	JCore::NormalLock m_Lock;

	// 2가지 방식으로 관리
	Serial$DataMap m_hmSerialMap;			// 발급된 시리얼과 토큰 데이터를 묶음
	DateTime$DataMap m_tmTimeMap;			// 발급된 시각과 토큰 데이터를 묶음 (일정 주기마다 빠르게 토큰 만료처리를 하기위함)
	AccountId$DataMap m_hmAccountIdMap;		// 어떤 아이디들에게 토큰이 발급되었는지
};
