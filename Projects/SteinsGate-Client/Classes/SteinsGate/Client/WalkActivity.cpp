/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "WalkActivity.h"

#include <SteinsGate/Client/AIComponent.h>
#include <SteinsGate/Client/MoveComponent.h>
#include <SteinsGate/Client/SGVec2Ex.h>

#define DestinationMinDistX 7.0f	// 목표지점까지 거리가 이정도 이내이면 도착했다고 판정
#define DestinationMinDistY 5.0f

WalkActivity::WalkActivity(Actor* actor)
	: AIActivity(actor, AIActivityType::Walk)
	, m_pTarget(nullptr)
	, m_eMode(eNone)
{}

void WalkActivity::onActivitySelectFromAIRoutine(AIInfo* aiInfo, AIState_t aiState) {
	AIComponent* pAIComponent = m_pActor->getComponent<AIComponent>();
	DebugAssert(pAIComponent);// AIComponent의해 실행된 함수인데, nullptr일 수가 없음

	switch (aiState) {
	case AIState::Wander: {
		m_fLimitTime = SGRandom::random_real(aiInfo->WanderWalkTime[0], aiInfo->WanderWalkTime[1]);
		m_Destination = pAIComponent->getRandomSightPos();
		m_eMode = eWander;
		break;
	}
	case AIState::Track: {
		m_fLimitTime = SGRandom::random_real(aiInfo->TrackWalkTime[0], aiInfo->TrackWalkTime[1]);
		m_eMode = eTrack;
		break;
	}
	}
}

void WalkActivity::onUpdate(float dt) {
	updateLimitTime(dt);

	if (!isRunning())
		return;

	switch (m_eMode) {
		case eNone: return;
		case eWander: updateWander(dt); break;
		case eTrack: updateTrack(dt); break;
		default: DebugAssertMsg(false, "몬스터 Walking 액티비티 모드가 이상합니다.");
	}

	updateMove(dt);
}

void WalkActivity::updateWander(float dt) {
	
}

void WalkActivity::updateTrack(float dt) {
	if (m_pTarget == nullptr)
		return;

	m_Destination =  m_pTarget->getPositionRealCenter();
}

void WalkActivity::updateMove(float dt) {
	Direction_t lr;
	Direction_t ud;
	SGRect thicknessPosLR = m_pActor->getThicknessBoxRect();
	SGRect thicknessPosUD = thicknessPosLR;
	SGVec2 center = thicknessPosLR.getMid();

	SGVec2Ex::getLookDirection(center, m_Destination, lr, ud);

	bool bArrivedX = false;
	bool bArrivedY = false;

	if (SGMath::Abs(center.x - m_Destination.x) < DestinationMinDistX)
		bArrivedX = true;

	if (SGMath::Abs(center.y - m_Destination.y) < DestinationMinDistY)
		bArrivedY = true;

	MoveComponent* pMoveComponent = m_pActor->getComponent<MoveComponent>();
	
	if (bArrivedX && bArrivedY) {

		if (pMoveComponent) 
			pMoveComponent->setSpeed(0, 0);

		stop();
		return;
	}

	if (!pMoveComponent)
		return;

	updateMoveImpl(dt, pMoveComponent, bArrivedX, bArrivedX, lr, ud);
}




