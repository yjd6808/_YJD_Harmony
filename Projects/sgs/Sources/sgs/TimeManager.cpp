/*
 * 작성자: 윤정도
 * 생성일: 7/9/2023 8:31:37 PM
 * =====================
 *
 */

#include "Core.h"
#include "ServerCoreHeader.h"
#include "TimeManager.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
TimeManager::TimeManager()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
TimeManager::~TimeManager()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void TimeManager::UpdateMasterServerTime(const DateTime& _masterServerTime)
{
	masterServerTime_ = _masterServerTime.Tick;
	timeDiff_ = DateTime::Now().Diff(_masterServerTime).Tick /* - 중앙 서버에서 수신하는데까지 걸리는 시간 */;

	// TODO: 중앙 서버에서 TimeSyncAck를 수신하는데까지 걸리는시간을 어케 구하지? RTT / 2를 해야하나?
	// 우선 동일한 LAN에서 서버를 열거라 왠만하면 이 시간은 매우 짧을 것이므로 0이라고 가정하자..
}

//////////////////////////////////////////////////////////////////////////////////////////
DateTime TimeManager::Now() const
{
	const DateTime now = DateTime::Now();
	const DateTime synchronizedNow = now.Tick - timeDiff_.Load();
	return synchronizedNow;
}
