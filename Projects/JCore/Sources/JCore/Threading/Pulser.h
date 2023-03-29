/*
 * 작성자: 윤정도
 * 생성일: 3/29/2023 6:49:15 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Time.h>

NS_JC_BEGIN

struct Pulser
{
	using PulserWatch = StopWatch<StopWatchMode::System>;

	Pulser(Int32U interval);
	int Wait();

	PulserWatch Watch;
	Int32U Interval;
};


NS_JC_END