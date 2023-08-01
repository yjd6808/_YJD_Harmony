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

class AuthTokenManager : public JCore::SingletonPointer<AuthTokenManager>
{
	using AccountId$TokenDataMap = JCore::TreeMap<int, AuthToken*>;
	using Token$TokenDataMap = JCore::HashMap<AuthTokenSerial_t, AuthToken*>;

	friend class TSingleton;
	AuthTokenManager() = default;
	~AuthTokenManager() override = default;
public:
	bool Issue(int accountTableId, const char* id);			// 발급
	bool CheckBySerial(AuthTokenSerial_t serial, const char* id);	// 발급되었는지 확인
	bool CheckByTableId(int accountTableId, const char* id);
	bool UpdateBySerial(AuthTokenSerial_t serial);									// 최신화시각 갱신
	bool UpdateByTableId(int accountTableId);
	void Clear();
	bool RemoveBySerial(AuthTokenSerial_t serial);
	bool RemoveByTableId(int accountTableId);

	void OnScheduled(JCore::SchedulerTask* task);								// 스케쥴링 될때마다 수행할 작업
private:
	bool IssueRaw(int accountTableId, const char* id);
	bool CheckRawBySerial(AuthTokenSerial_t serial);
	bool CheckRawByTableId(int accountTableId);
	bool CheckRawBySerial(AuthTokenSerial_t serial, const char* id);
	bool CheckRawByTableId(int accountTableId, const char* id);
	bool UpdateRawBySerial(AuthTokenSerial_t serial);
	bool UpdateRawByTableId(int accountTableId);
	bool RemoveRawBySerial(AuthTokenSerial_t serial);
	bool RemoveRawByTableId(int accountTableId);

	void GenerateSerial(JCORE_OUT AuthTokenSerial_t& token);
private:
	JCore::NormalLock m_Lock;

	// 2가지 방식으로 관리
	AccountId$TokenDataMap m_tmAccountTableIdMap;	// 발급된 토큰 데이터를 DB테이블의 계정 고유ID와 묶어서 관리
	Token$TokenDataMap m_tmSerialMap;				// 발급된 토큰 데이터를 인덱싱하여 관리
};