/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 2:52:37 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGConfig.h>

struct SGAIInfo
{
	SGAIInfo() = default;
	SGAIInfo(const SGAIInfo& other);

	// 합친 확률 분해해서 얻기
	float getWanderProb(AIWanderDecision_t decision);
	float getTrackProb(AITrackDecision_t decision);
	float getAngryProb(AIAngryDecision_t decision);


	int Code;
	float WanderProbs[AIWanderDecision::Max - 1];
	float TrackProbs[AITrackDecision::Max - 1];
	float AngryProbs[AIAngryDecision::Max - 1];

	float IdleTime[2];			// min, max
	float WanderWalkTime[2];
	float FollowWalkTime[2];

	bool ForceTrack;
	bool ForceAngry;

	float SightRadious;
	float AttackRadious;		// TODO: 인스턴트 히트박스로 변경필요
};

