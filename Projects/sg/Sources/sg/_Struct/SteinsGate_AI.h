/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 2:52:37 PM
 * =====================
 *
 */

#pragma once

#include <sg/_Util/DescBase.h>

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(AIState)
	None,
	Wander,
	Track,
	Angry
JC_SENUM_END(AIState)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(AIWanderDecision)
	Walk,
	Idle,
	Max
JC_SENUM_END(AIWanderDecision)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(AITrackDecision)
	Wander,
	Attack,
	Follow,
	Max
JC_SENUM_END(AITrackDecision)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(AIAngryDecision)
	Wander,
	Track,
	Attack,
	Max
JC_SENUM_END(AIAngryDecision)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(AIActivityType)
		Idle,
		Walk,
		Attack,
		Hit,
		FallDown,
		Sit,
		// TODO: Custom 몬스터 특수행동 그런거 추가할 수 있을 듯?
		Max,
		None
JC_SENUM_MIDDLE(AIActivityType)
static constexpr const char* Name[Max]{
	"Idle",
	"Walk",
	"Attack",
	"Hit",
	"FallDown",
	"Sit"
};

JC_SENUM_MIDDLE_END(AIActivityType)

//////////////////////////////////////////////////////////////////////////////////////////
struct AIInfo : SDescBase
{
	AIInfo() = default;
	AIInfo(const AIInfo& _other);
	~AIInfo() override = default;

	// 합친 확률 분해해서 얻기
	float GetWanderProb(AIWanderDecision_t _decision);
	float GetTrackProb(AITrackDecision_t _decision);
	float GetAngryProb(AIAngryDecision_t _decision);

	float wanderProbs_[AIWanderDecision::Max - 1];
	float trackProbs_[AITrackDecision::Max - 1];
	float angryProbs_[AIAngryDecision::Max - 1];

	float idleTime_[2]; // min, max
	float wanderWalkTime_[2];
	float trackWalkTime_[2];

	bool forceTrack_;
	bool forceAngry_;

	float sightRadious_;
	float attackRadious_; // TODO: 인스턴트 히트박스로 변경필요
};
