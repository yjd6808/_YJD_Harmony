/*
 * 작성자: 윤정도
 * 생성일: 8/25/2023 7:49:03 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Define.h>

JCORE_SENUM_BEGIN(AIState)
None,
Wander,
Track,
Angry
JCORE_SENUM_END(AIState)

// ==========================================================================================

JCORE_SENUM_BEGIN(AIWanderDecision)
Walk,
Idle,
Max
JCORE_SENUM_END(AIWanderDecision)

// ==========================================================================================

JCORE_SENUM_BEGIN(AITrackDecision)
Wander,
Attack,
Follow,
Max
JCORE_SENUM_END(AITrackDecision)

// ==========================================================================================

JCORE_SENUM_BEGIN(AIAngryDecision)
Wander,
Track,
Attack,
Max
JCORE_SENUM_END(AIAngryDecision)

// ==========================================================================================

JCORE_SENUM_BEGIN(AIActivityType)
Idle,
Walk,
Attack,
Hit,
FallDown,
Sit,
// TODO: Custom 몬스터 특수행동 그런거 추가할 수 있을 듯?
Max,
None
JCORE_SENUM_MIDDLE(AIActivityType)
static constexpr const char* Name[Max]{
	"Idle",
	"Walk",
	"Attack",
	"Hit",
	"FallDown",
	"Sit"
};

JCORE_SENUM_MIDDLE_END(AIActivityType)

// ==========================================================================================