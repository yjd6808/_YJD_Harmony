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


#include <jc/Threading/Scheduler.h>

#include <jc/Container/TreeMap.h>
#include <jc/Container/HashMap.h>

class AuthenticationManager : public jc::SingletonPointer<AuthenticationManager>
{
	using SerialDataMap = jc::HashMap<AuthenticationSerial_t, AuthenticationData*>;
	using AccountIdDataMap = jc::HashMap<jc::String, AuthenticationData*>;
	using DateTimeDataMap = jc::TreeMap<jc::DateTime, AuthenticationData*>;

	friend class TSingleton;
	AuthenticationManager() = default;
	~AuthenticationManager() = default;

public:
	AuthenticationData* Issue(const  AccountData& _accountData);								// 발급
	bool Exist(AuthenticationSerial_t _serial, const char* _pAccountId);							// 발급되었는지 확인
	AuthenticationData* Update(AuthenticationSerial_t _serial, const char* _pAccountId, AuthenticationState_t _state);	// 최신화시각 갱신 및 상태 업데이트
	bool Remove(AuthenticationSerial_t _serial, const char* _pAccountId);
	void Clear();
	void OnScheduled(jc::SchedulerTask* _pTask);

	class Schedule : public jc::SchedulerTaskRunnable
	{
	public:
		void OnFirstScheduled() override {}
		void OnScheduled() override;

		void Initialize() override {}
		void Finalize() override {}

		void Disable() { isRunning_ = false; }

		jc::TimeSpan Interval() override
		{
			return jc::TimeSpan::FromMiliSeocnd(1);
		}
	private:
		jc::AtomicBool isRunning_ = true;
	};
private:
	AuthenticationData* IssueRaw(const  AccountData& _accountData);
	AuthenticationData* FindRaw(const jc::DateTime& _timeId);
	AuthenticationData* FindRaw(AuthenticationSerial_t _serial);
	AuthenticationData* FindRaw(AuthenticationSerial_t _serial, const char* _pAccountId);
	AuthenticationData* UpdateRaw(AuthenticationSerial_t _serial, const char* _pAccountId, AuthenticationState_t _nextState);
	bool RemoveRaw(const jc::DateTime& _timeId);
	bool RemoveRaw(AuthenticationSerial_t _serial);
	bool RemoveRaw(AuthenticationSerial_t _serial, const char* _pAccountId);

	bool GenerateSerial(OUT AuthenticationSerial_t& _serial);
	bool GenerateTimeId(OUT jc::DateTime& _timeId, AuthenticationState_t _state);

private:
	mutable jc::NormalLock lock_;

	SerialDataMap serialDataMap_;		// 발급된 시리얼과 토큰 데이터를 묶음
	DateTimeDataMap timeDataMap_;		// 발급된 시각과 토큰 데이터를 묶음 (일정 주기마다 빠르게 토큰 만료처리를 하기위함)
	AccountIdDataMap accountIdDataMap_;	// 어떤 아이디들에게 토큰이 발급되었는지
};

#define g_cAuthMgr JC_DECL_SINGLETON_BODY(AuthenticationManager)