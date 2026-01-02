/*
 * 작성자: 윤정도
 * 생성일: 3/29/2023 6:51:07 PM
 * =====================
 *
 */

#include <jc/Core.h>

#include <jc/Threading/Thread.h>
#include <jc/Threading/Pulser.h>

NS_JC_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////
Pulser::Pulser(Int32U _intervalMiliseconds, Mode _mode /* = eSliceCycle */,  PulserStatistics* _pStatistics /* = nullptr */)
: interval_(_intervalMiliseconds)
, pStatistics_(_pStatistics)
{
	switch (_mode)
	{
	case eSliceCycle: pWaitor_ = dbg_new SliceWaitor(*this);
		break;
	case eExactCycle: pWaitor_ = dbg_new ExactWaitor(*this);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
Pulser::~Pulser()
{
	JC_DELETE_SAFE(pWaitor_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Pulser::Start()
{
	watch_.Start();
}

//////////////////////////////////////////////////////////////////////////////////////////
TimeSpan Pulser::Wait()
{
	return pWaitor_->Wait();
}

//////////////////////////////////////////////////////////////////////////////////////////
TimeSpan Pulser::SliceWaitor::Wait()
{
	TimeSpan workTime = pulser_.watch_.GetElapsed();

	for (;;)
	{
		const auto workTimeMs = (Int32U)workTime.GetTotalMiliSecondsInt();
		if (workTimeMs >= pulser_.interval_)
		{
			break;
		}

		Thread::Sleep(1);
		workTime = pulser_.watch_.GetElapsed();
	}

	if (pulser_.pStatistics_)
	{
		pulser_.pStatistics_->pulseCount_ += 1;
		pulser_.pStatistics_->sleepIntervalLast_ = (Int32U)workTime.GetTotalMiliSecondsInt();
		pulser_.pStatistics_->totalSleepTime_ += pulser_.pStatistics_->sleepIntervalLast_;
	}

	pulser_.watch_.Start();
	return workTime;
}

//////////////////////////////////////////////////////////////////////////////////////////
TimeSpan Pulser::ExactWaitor::Wait()
{
	TimeSpan workTime = pulser_.watch_.GetElapsed();
	Int32U workTimeMs = (Int32U)workTime.GetTotalMiliSecondsInt();

	if (workTimeMs <= pulser_.interval_)
	{
		Thread::Sleep(pulser_.interval_ - workTimeMs);
	}
	else
	{
		// 펄스 주기를 넘어서는 경우 배수를 맞춰 줘야하므로 갭보다 큰 인터벌 카운트를 찾는다.
		// 예를들어서 1초 인터벌인데 1.5초동안 작업을 수행해버린 경우 1.5초를 쉬게되면 타이머가 꼬이므로 2초를 쉬어줘야한다.
		const int iIntervalCount = workTimeMs / pulser_.interval_ + 1;
		Thread::Sleep(iIntervalCount * pulser_.interval_ - workTimeMs);
	}
	workTime = pulser_.watch_.GetElapsed();

	if (pulser_.pStatistics_)
	{
		pulser_.pStatistics_->pulseCount_ += 1;
		pulser_.pStatistics_->sleepIntervalLast_ = (Int32U)workTime.GetTotalMiliSecondsInt();
		pulser_.pStatistics_->totalSleepTime_ += pulser_.pStatistics_->sleepIntervalLast_;
	}

	pulser_.watch_.Start();
	return workTime;
}

NS_JC_END
