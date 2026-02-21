/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Core.h"
#include "WalkActivity.h"

#include <sgcl/AIComponent.h>
#include <sgcl/MoveComponent.h>
#include <sgcl/SGVec2Ex.h>

#define DestinationMinDistX 7.0f	// 목표지점까지 거리가 이정도 이내이면 도착했다고 판정
#define DestinationMinDistY 5.0f

//////////////////////////////////////////////////////////////////////////////////////////
WalkActivity::WalkActivity(Actor* _pActor)
: AIActivity(_pActor, AIActivityType::Walk)
, pTarget_(nullptr)
, mode_(eNone)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void WalkActivity::OnActivitySelectFromAiRoutine(AIInfo* _pAiInfo, AIState_t _aiState)
{
	AIComponent* pAIComponent = pActor_->GetComponent<AIComponent>();
	jc_assert(pAIComponent); // AIComponent의해 실행된 함수인데, nullptr일 수가 없음

	switch (_aiState)
	{
	case AIState::Wander:
		{
			limitTime_ = g_cRandom.GenerateF(_pAiInfo->wanderWalkTime_[0], _pAiInfo->wanderWalkTime_[1]);
			destination_ = pAIComponent->GetRandomSightPos();
			mode_ = eWander;
			break;
		}
	case AIState::Track:
		{
			limitTime_ = g_cRandom.GenerateF(_pAiInfo->trackWalkTime_[0], _pAiInfo->trackWalkTime_[1]);
			mode_ = eTrack;
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

	switch (mode_)
	{
	case eNone:
		return;
	case eWander:
		UpdateWander(_dt);
		break;
	case eTrack:
		UpdateTrack(_dt);
		break;
	default:
		jc_assert_msg(false, "몬스터 Walking 액티비티 모드가 이상합니다.");
		break;
	}

	UpdateMove(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void WalkActivity::UpdateWander(float _dt)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void WalkActivity::UpdateTrack(float _dt)
{
	if (pTarget_ == nullptr)
		return;

	destination_ = pTarget_->GetPositionRealCenter();
}

//////////////////////////////////////////////////////////////////////////////////////////
void WalkActivity::UpdateMove(float _dt)
{
	Direction_t lr;
	Direction_t ud;
	cc::rect thicknessPosLR = pActor_->GetThicknessBoxRect();
	cc::rect thicknessPosUD = thicknessPosLR;
	cc::vec2 center = thicknessPosLR.getMid();

	SGVec2Ex::GetLookDirection(center, destination_, lr, ud);

	bool arrivedX = false;
	bool arrivedY = false;

	if (jc::Math::Abs(center.x - destination_.x) < DestinationMinDistX)
		arrivedX = true;

	if (jc::Math::Abs(center.y - destination_.y) < DestinationMinDistY)
		arrivedY = true;

	MoveComponent* pMoveComponent = pActor_->GetComponent<MoveComponent>();

	if (arrivedX && arrivedY)
	{
		if (pMoveComponent)
		{
			pMoveComponent->SetSpeed(0, 0);
		}

		Stop();
		return;
	}

	if (!pMoveComponent)
		return;

	UpdateMoveImpl(_dt, pMoveComponent, arrivedX, arrivedX, lr, ud);
}
