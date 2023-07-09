/*
 * 작성자: 윤정도
 * 생성일: 7/9/2023 7:42:21 PM
 * =====================
 * 피어 서버들이 중앙 서버와 시간 정보를 기록한다.
 * 매번 정확한 시간 정보를 얻기 위해 중앙 서버에 시간정보를 물어보는건 엄청난 낭비이다.
 * 따라서 피어 서버는 중앙 서버와의 시간 차이를 기록하여 물어보지 않고 중앙 서버의 시각정보를 계산할 수 있도록 한다.
 * 시간 차이 = TimeSyncAck 패킷을 받았을 때의 시각 - (중앙 서버에서 시각 + 중앙서버에서 피어서버로 패킷이 돌아오는데 걸리는 시간)
 * 
 *                  ┌──── b ────┐
 * 3. 시각 정보 확인  ↓           ↑ 2. 시간 정보 요청 응답
 *                Peer		  Center
 * 1. 시간 정보 요청  ↓           ↑
 *                  └──── a ────┘ 
 */


#pragma once

#include <JCore/Time.h>
#include <JCore/Primitives/Atomic.h>
#include <JCore/Pattern/Singleton.h>

class TimeManager : public JCore::SingletonPointer<TimeManager>
{
private:
	friend class TSingleton;
	TimeManager();
	~TimeManager() override = default;
public:
	void UpdateMasterServerTime(const JCore::DateTime& masterServerTime);
	JCore::DateTime Now() const;
private:
	JCore::AtomicInt64 m_MasterServerTime;
	JCore::AtomicInt64 m_TimeDiff;			// 피어 서버와 마스터 서버의 시간 차이
};

