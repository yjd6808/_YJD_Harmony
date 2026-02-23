/*
 * 작성자: 윤정도
 * 생성일: 5/5/2023 9:51:02 AM
 * =====================
 * 1. 클라이언트 로컬 시각정보가 필요할 때 GetSystemTick() 같은걸로 사용해도 상관없긴하다.
 *    근데 시간 계산을 해야할 경우 코드 작성이 번거로울 수 있으므로.. TimeSpan과 DateTime같은 래퍼클래스로 표현하였다.
 *     => 따라서 모든 시간단위는 기본적으로 마이크로초단위이다. 하지만 실제 정밀도는 밀리초 단위이다.
 * 2. App은 클라이언트를 의미한다.
 */


#pragma once



#include <sgcl/Core.h>

class TimeManager : public jc::SingletonPointer<TimeManager>
{
private:
	friend class TSingleton;
	TimeManager();
	~TimeManager();

public:
	void UpdateAppTime();
	void UpdateServerTime();

	_s64 GetAppStartUpTick() const
	{
		return appStartupTime_.Tick;
	}

	_s64 GetAppTick() const
	{
		return appTime_.Tick;
	}

	const jc::DateTime& GetAppTime() const
	{
		return appTime_;
	}

	const jc::TimeSpan& GetElapsedAppTime() const
	{
		return appRunningTime_;
	}

private:
	jc::TimeSpan appStartupSystemTime_; // App 시작 당시 OS 부팅 후 경과 시간
	jc::DateTime appStartupTime_; // App 시작 시각
	jc::DateTime appTime_; // App 현재 시각
	jc::TimeSpan appRunningTime_; // App 시작 후 경과 시간

	jc::DateTime serverTime_;

	bool lobbyServerTimeUpdated_;
	bool gameServerTimeUpdated_;
};
