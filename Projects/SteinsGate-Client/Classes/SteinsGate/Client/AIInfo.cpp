/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 3:04:07 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "AIInfo.h"

#include <JCore/Assert.h>

//////////////////////////////////////////////////////////////////////////////////////////
AIInfo::AIInfo(const AIInfo& _other)
{
	Code = _other.Code;

	for (int i = 0; i < AIWanderDecision::Max - 1; ++i)
	{
		WanderProbs[i] = _other.WanderProbs[i];
	}

	for (int i = 0; i < AITrackDecision::Max - 1; ++i)
	{
		TrackProbs[i] = _other.TrackProbs[i];
	}

	for (int i = 0; i < AIAngryDecision::Max - 1; ++i)
	{
		AngryProbs[i] = _other.AngryProbs[i];
	}

	for (int i = 0; i < 2; ++i)
	{
		IdleTime[i] = _other.IdleTime[i];
		WanderWalkTime[i] = _other.WanderWalkTime[i];
		TrackWalkTime[i] = _other.TrackWalkTime[i];
	}

	ForceTrack = _other.ForceTrack;
	ForceAngry = _other.ForceAngry;
	SightRadious = _other.SightRadious;
	AttackRadious = _other.AttackRadious;
}

//////////////////////////////////////////////////////////////////////////////////////////
float AIInfo::getWanderProb(AIWanderDecision_t _decision)
{
	DebugAssertMsg(_decision >= 0 && _decision < AIWanderDecision::Max, "원더 디시전 범위가 이상합니다.");
	if (_decision == 0)
		return WanderProbs[0];

	return WanderProbs[_decision] - WanderProbs[_decision - 1];
}

//////////////////////////////////////////////////////////////////////////////////////////
float AIInfo::getTrackProb(AITrackDecision_t _decision)
{
	DebugAssertMsg(_decision >= 0 && _decision < AITrackDecision::Max, "트랙 디시전 범위가 이상합니다.");
	if (_decision == 0)
		return TrackProbs[0];

	return TrackProbs[_decision] - TrackProbs[_decision - 1];
}

//////////////////////////////////////////////////////////////////////////////////////////
float AIInfo::getAngryProb(AIAngryDecision_t _decision)
{
	DebugAssertMsg(_decision >= 0 && _decision < AIAngryDecision::Max, "앵구리 디시전 범위가 이상합니다.");
	if (_decision == 0)
		return AngryProbs[0];

	return AngryProbs[_decision] - AngryProbs[_decision - 1];
}
