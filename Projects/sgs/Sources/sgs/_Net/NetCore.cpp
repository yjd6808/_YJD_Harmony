/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:28 PM
 * =====================
 *
 */

#include "Core.h"
#include "NetCore.h"

#include <sg/_API/sgapi_Util.h>
#include <sg/_Util/DescLoaderMgr.h>
#include <sg/_Struct/SteinsGate_Server.h>

#include <sgs/_Net/NetGroup_Main.h>
#include <sgs/_Net/NetGroup_InterServ.h>
#include <sgs/InterServerSendHelper.h>

USING_NS_JC;
USING_NS_JNET;
USING_NS_SG;

//////////////////////////////////////////////////////////////////////////////////////////
NetCore::NetCore()
: running_(true)
, pProcessInfo_(g_cDescMgr.GetServerProcessInfo())
{
	updatePerSecond_ = pProcessInfo_->updatePerSecond_;
	isInterServerClient_ = sgapi_IsInterServerClient(); // 멤버로 캐싱해줌.
	processType_ = sgapi_GetServerProcessType();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool NetCore::Initialize()
{
	AddNetGroup(NET_GID_MAIN, g_cNetGroup_Main.GetShared());
	AddNetGroup(NET_GID_INTER_SERV, g_cNetGroup_InterServ.GetShared());

	g_cNetGroup_Main.Initialize();

	if (isInterServerClient_)
	{
		g_cNetGroup_InterServ.Initialize();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetCore::ProcessMainLoop()
{
	PulserStatistics pulseStat;
	Pulser pulser(1000 / updatePerSecond_, Pulser::eSliceCycle, &pulseStat);
	TimeSpan elapsed;

	pulser.Start();
	while (running_)
	{
		OnUpdate(elapsed);
		elapsed = pulser.Wait();
		// Console::WriteLine("%d", elapsed.GetTotalMiliSecondsInt());
	}

	Finalize();

	if (fnStopped_)
		fnStopped_();
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetCore::UpdateServerTime(Int64 _sendTime, Int64 _centerTime)
{
	if (_centerTime < centerServerTime_) // 시간 역행 금지
		return;

	Int64 now = DateTime::Now().Tick;
	Int64 rtt = now - _sendTime;

	centerServerPrevTime_ = centerServerTime_;
	centerServerTime_ = _centerTime;
	centerServerTimeDiff_ = centerServerTime_ - now;

	// ping(rtt) 시간 계산
	// 최근 최대 10개의 rtt값의 평균을 ping으로 계산하도록 한다.
	// slinding window 처리
	if (rttQueueSize_ < RTT_QUEUE_MAX_SIZE)
	{
		rttSum_ += rtt;
		rttQueue_[rttQueueIndex_++] = rtt;
		++rttQueueSize_;
	}
	else
	{
		rttSum_ -= rttQueue_[rttQueueIndex_];
		rttSum_ += rtt;
		rttQueue_[rttQueueIndex_++] = rtt;
	}

	if (rttQueueIndex_ >= RTT_QUEUE_MAX_SIZE)
		rttQueueIndex_ = 0;

	avgRtt_ = rttSum_ / rttQueueSize_;
}

//////////////////////////////////////////////////////////////////////////////////////////
Int64 NetCore::GetTime()
{
	Int64 now = DateTime::Now().Tick;
	Int64 serverTime = now + centerServerTimeDiff_; // 오차만큼 더해줌.
	return serverTime;
}

//////////////////////////////////////////////////////////////////////////////////////////
void NetCore::OnUpdate(const TimeSpan& _elapsed)
{
	g_cVM.Go(_elapsed);

	g_cNetGroup_Main.OnUpdate(_elapsed);

	if (isInterServerClient_)
	{
		g_cNetGroup_InterServ.OnUpdate(_elapsed);
	}

	if (fnUpdate_)
		fnUpdate_(_elapsed);
}
