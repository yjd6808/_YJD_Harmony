/*
 * 작성자: 윤정도
 * 생성일: 5/5/2023 10:36:06 AM
 * =====================
 *
 */


#include "Core.h"
#include "TimeManager.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
TimeManager::TimeManager()
: appStartupSystemTime_(Env::SystemTime())
, appStartupTime_(DateTime::Now())
, lobbyServerTimeUpdated_(false)
, gameServerTimeUpdated_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
TimeManager::~TimeManager()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void TimeManager::UpdateAppTime()
{
	// 코코스엔진의 Application::run() 함수를 보면
	// 시간 해상도를 1ms로 설정해서 사용하기 때문에 GetTickCount 보다 더 높은 정밀도를 보여준다.
	const Int64 elapsedAppTime = Int64(::timeGetTime()) * 1'000 - appStartupSystemTime_.Tick;

	appTime_.Tick = appStartupTime_.Tick + elapsedAppTime;
	appRunningTime_.Tick = elapsedAppTime;
}

//////////////////////////////////////////////////////////////////////////////////////////
void TimeManager::UpdateServerTime()
{
}
