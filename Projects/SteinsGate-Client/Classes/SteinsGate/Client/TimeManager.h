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


#include <JCore/Pattern/Singleton.h>
#include <SteinsGate/Client/Tutturu.h>

class TimeManager : public JCore::SingletonPointer<TimeManager>
{
private:
	friend class TSingleton;
	TimeManager();
	~TimeManager();

public:
	void updateAppTime();
	void updateServerTime();

	Int64 getAppStartUpTick() const
	{
		return appStartupTime_.Tick;
	}

	Int64 getAppTick() const
	{
		return appTime_.Tick;
	}

	const SGDateTime& getAppTime() const
	{
		return appTime_;
	}

	const SGTimeSpan& getElapsedAppTime() const
	{
		return appRunningTime_;
	}

private:
	SGTimeSpan appStartupSystemTime_; // App 시작 당시 OS 부팅 후 경과 시간
	SGDateTime appStartupTime_; // App 시작 시각
	SGDateTime appTime_; // App 현재 시각
	SGTimeSpan appRunningTime_; // App 시작 후 경과 시간

	SGDateTime serverTime_;

	bool lobbyServerTimeUpdated_;
	bool gameServerTimeUpdated_;
};
