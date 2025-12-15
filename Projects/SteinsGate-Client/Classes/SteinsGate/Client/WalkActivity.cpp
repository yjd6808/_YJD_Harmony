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

//////////////////////////////////////////////////////////////////////////////////////////
WalkActivity::WalkActivity(Actor* _pActor)
: AIActivity(_pActor, AIActivityType::Walk)
, m_pTarget(nullptr)
, m_eMode(eNone)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void WalkActivity::OnActivitySelectFromAiRoutine(AIInfo* _pAiInfo, AIState_t _aiState)
{
	AIComponent* pAIComponent = pActor_->getComponent<AIComponent>();
	DebugAssert(pAIComponent); // AIComponent의해 실행된 함수인데, nullptr일 수가 없음

	switch (_aiState)
	{
	case AIState::Wander:
		{
			limitTime_ = SGRandom::random_real(_pAiInfo->wanderWalkTime_[0], _pAiInfo->wanderWalkTime_[1]);
			m_Destination = pAIComponent->getRandomSightPos();
			m_eMode = eWander;
			break;
		}
	case AIState::Track:
		{
			limitTime_ = SGRandom::random_real(_pAiInfo->trackWalkTime_[0], _pAiInfo->trackWalkTime_[1]);
			m_eMode = eTrack;
			break;
		}
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void WalkActivity::OnUpdate(float _dt)
{
	UpdateLimitTime(_dt);

	if (!IsRunning())
		return;

	switch (m_eMode)
	{
	case eNone:
		return;
	case eWander:
		updateWander(_dt);
		break;
	case eTrack:
		updateTrack(_dt);
		break;
	default:
		DebugAssertMsg(false, "몬스터 Walking 액티비티 모드가 이상합니다.");
		break;
	}

	updateMove(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void WalkActivity::updateWander(float _dt)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void WalkActivity::updateTrack(float _dt)
{
	if (m_pTarget == nullptr)
		return;

	m_Destination = m_pTarget->getPositionRealCenter();
}

//////////////////////////////////////////////////////////////////////////////////////////
void WalkActivity::updateMove(float _dt)
{
	Direction_t lr;
	Direction_t ud;
	SGRect thicknessPosLR = pActor_->getThicknessBoxRect();
	SGRect thicknessPosUD = thicknessPosLR;
	SGVec2 center = thicknessPosLR.getMid();

	SGVec2Ex::getLookDirection(center, m_Destination, lr, ud);

	bool arrivedX = false;
	bool arrivedY = false;

	if (SGMath::Abs(center.x - m_Destination.x) < DestinationMinDistX)
		arrivedX = true;

	if (SGMath::Abs(center.y - m_Destination.y) < DestinationMinDistY)
		arrivedY = true;

	MoveComponent* pMoveComponent = pActor_->getComponent<MoveComponent>();

	if (arrivedX && arrivedY)
	{
		if (pMoveComponent)
		{
			pMoveComponent->setSpeed(0, 0);
		}

		Stop();
		return;
	}

	if (!pMoveComponent)
		return;

	updateMoveImpl(_dt, pMoveComponent, arrivedX, arrivedX, lr, ud);
}
