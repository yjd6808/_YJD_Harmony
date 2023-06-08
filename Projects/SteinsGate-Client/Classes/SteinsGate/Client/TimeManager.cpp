/*
 * 작성자: 윤정도
 * 생성일: 5/5/2023 10:36:06 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "TimeManager.h"

USING_NS_JC;

TimeManager::TimeManager()
	: m_AppStartupSystemTime(Env::SystemTime())
	, m_AppStartupTime(DateTime::Now())
	, m_bLobbyServerTimeUpdated(false)
	, m_bGameServerTimeUpdated(false)
{}

TimeManager::~TimeManager() {}


void TimeManager::updateAppTime() {

	// 코코스엔진의 Application::run() 함수를 보면
	// 시간 해상도를 1ms로 설정해서 사용하기 때문에 GetTickCount 보다 더 높은 정밀도를 보여준다.
	const Int64 iElpasedAppTime = Int64(::timeGetTime()) * 1'000 - m_AppStartupSystemTime.Tick;

	m_AppTime = m_AppStartupTime.Tick + iElpasedAppTime;
	m_AppRunningTime = iElpasedAppTime;
}
