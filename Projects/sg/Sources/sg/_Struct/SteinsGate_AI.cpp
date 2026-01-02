/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 3:04:07 PM
 * =====================
 *
 */

#include "Core.h"
#include "SteinsGate_AI.h"

#include <jc/Assert.h>

//////////////////////////////////////////////////////////////////////////////////////////
AIInfo::AIInfo(const AIInfo& _other)
{
	code_ = _other.code_;

	for (int i = 0; i < AIWanderDecision::Max - 1; ++i)
	{
		wanderProbs_[i] = _other.wanderProbs_[i];
	}

	for (int i = 0; i < AITrackDecision::Max - 1; ++i)
	{
		trackProbs_[i] = _other.trackProbs_[i];
	}

	for (int i = 0; i < AIAngryDecision::Max - 1; ++i)
	{
		angryProbs_[i] = _other.angryProbs_[i];
	}

	for (int i = 0; i < 2; ++i)
	{
		idleTime_[i] = _other.idleTime_[i];
		wanderWalkTime_[i] = _other.wanderWalkTime_[i];
		trackWalkTime_[i] = _other.trackWalkTime_[i];
	}

	forceTrack_ = _other.forceTrack_;
	forceAngry_ = _other.forceAngry_;
	sightRadious_ = _other.sightRadious_;
	attackRadious_ = _other.attackRadious_;
}

//////////////////////////////////////////////////////////////////////////////////////////
float AIInfo::GetWanderProb(AIWanderDecision_t _decision)
{
	jc_assert_msg(_decision >= 0 && _decision < AIWanderDecision::Max, "원더 디시전 범위가 이상합니다.");
	if (_decision == 0)
	{
		return wanderProbs_[0];
	}

	return wanderProbs_[_decision] - wanderProbs_[_decision - 1];
}

//////////////////////////////////////////////////////////////////////////////////////////
float AIInfo::GetTrackProb(AITrackDecision_t _decision)
{
	jc_assert_msg(_decision >= 0 && _decision < AITrackDecision::Max, "트랙 디시전 범위가 이상합니다.");
	if (_decision == 0)
	{
		return trackProbs_[0];
	}

	return trackProbs_[_decision] - trackProbs_[_decision - 1];
}

//////////////////////////////////////////////////////////////////////////////////////////
float AIInfo::GetAngryProb(AIAngryDecision_t _decision)
{
	jc_assert_msg(_decision >= 0 && _decision < AIAngryDecision::Max, "앵구리 디시전 범위가 이상합니다.");
	if (_decision == 0)
	{
		return angryProbs_[0];
	}

	return angryProbs_[_decision] - angryProbs_[_decision - 1];
}
