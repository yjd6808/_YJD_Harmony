/*
 * 작성자: 윤정도
 * 생성일: 3/29/2023 6:49:15 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Time.h>
#include <JCore/Pattern/NonCopyableh.h>

NS_JC_BEGIN

struct PulserStatistics
{
	PulserStatistics()
		: SleepIntervalLast(0)
		, TotalSleepTime(0)
		, PulseCount(0)
	{}

	Int32U SleepIntervalLast;		// 마지막으로 실제로 Sleep한 시간
	Int32U TotalSleepTime;			// 얼마나 잤는지 (누계)
	Int32U PulseCount;				// 몇번 Sleep에서 깨어났는지 0으로 돌아가도록 unsigned로둠

	void Reset() {
		PulseCount = 0;
		SleepIntervalLast = 0;
		TotalSleepTime = 0;
	}
};

struct Pulser
{
	using PulserWatch = StopWatch<StopWatchMode::System>;

	Pulser(Int32U intervalMiliseconds, JCORE_IN PulserStatistics* statistics = nullptr);
	void Wait();

	Int32U Interval;
	PulserWatch Watch;
	PulserStatistics* Statistics;
};


NS_JC_END