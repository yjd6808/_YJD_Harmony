/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 3:04:07 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGAIInfo.h"

#include <JCore/Assert.h>

SGAIInfo::SGAIInfo(const SGAIInfo& other) {
	Code = other.Code;

	for (int i = 0; i < AIWanderDecision::Max - 1; ++i) {
		WanderProbs[i] = other.WanderProbs[i];
	}

	for (int i = 0; i < AITrackDecision::Max - 1; ++i) {
		TrackProbs[i] = other.TrackProbs[i];
	}

	for (int i = 0; i < AIAngryDecision::Max - 1; ++i) {
		AngryProbs[i] = other.AngryProbs[i];
	}

	for (int i = 0; i < 2; ++i) {
		IdleTime[i] = other.IdleTime[i];
		WanderWalkTime[i] = other.WanderWalkTime[i];
		FollowWalkTime[i] = other.FollowWalkTime[i];
	}

	ForceTrack = other.ForceTrack;
	ForceAngry = other.ForceAngry;
	SightRadious = other.SightRadious;
	AttackRadious = other.AttackRadious;
}

float SGAIInfo::getWanderProb(AIWanderDecision_t decision) {
	DebugAssertMsg(decision >= 0 && decision < AIWanderDecision::Max, "원더 디시전 범위가 이상합니다.");
	if (decision == 0)
		return WanderProbs[0];

	return WanderProbs[decision] - WanderProbs[decision - 1];
}
float SGAIInfo::getTrackProb(AITrackDecision_t decision) {
	DebugAssertMsg(decision >= 0 && decision < AITrackDecision::Max, "트랙 디시전 범위가 이상합니다.");
	if (decision == 0)
		return TrackProbs[0];

	return TrackProbs[decision] - TrackProbs[decision - 1];
}
float SGAIInfo::getAngryProb(AIAngryDecision_t decision) {
	DebugAssertMsg(decision >= 0 && decision < AIAngryDecision::Max, "앵구리 디시전 범위가 이상합니다.");
	if (decision == 0)
		return AngryProbs[0];

	return AngryProbs[decision] - AngryProbs[decision - 1];
}
