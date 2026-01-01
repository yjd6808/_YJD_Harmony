/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Core.h"
#include "HitActivity_Goblin.h"

#include <sgcl/Monster.h>
#include <sgcl/Define_Animation.h>
#include <sgcl/PhysicsComponent.h>
#include <sgcl/AIComponent.h>

//////////////////////////////////////////////////////////////////////////////////////////
HitActivity_Goblin::HitActivity_Goblin(Actor* _pActor)
: HitActivity(_pActor)
, hitSmall_(true)
, onTheGround_(false)
, downTimeCheckBegin_(false)
, elapsedDownTime_(0.0f)
, downRecoverTime_(0.0f)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitActivity_Goblin::OnActivityBegin()
{
	Monster* pMonster = dynamic_cast<Monster*>(pActor_);
	jc_assert(pMonster);

	const MonsterStatInfo* pStatInfo = pMonster->GetStatInfo();

	elapsedDownTime_ = 0.0f;
	downTimeCheckBegin_ = false;
	downRecoverTime_ = pStatInfo ? pStatInfo->downRecoverTime_ / 2 : 1.0f;

	SelectHitAnimation();
	CheckPosition();
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitActivity_Goblin::OnUpdate(float _dt)
{
	if (onTheGround_)
	{
		UpdateGroundHitState(_dt);
		return;
	}

	UpdateAirHitState(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitActivity_Goblin::SelectHitAnimation()
{
	if (hitSmall_)
	{
		pActor_->RunAnimation(DEF_ANIMATION_MONSTER_GOBLIN_HIT_SMALL);
	}
	else
	{
		pActor_->RunAnimation(DEF_ANIMATION_MONSTER_GOBLIN_HIT_BIG);
	}

	hitSmall_ = !hitSmall_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitActivity_Goblin::CheckPosition()
{
	PhysicsComponent* pPhysicsComponent = pActor_->GetComponent<PhysicsComponent>();

	if (!pPhysicsComponent->HasForceY() && pActor_->GetPositionActorY() <= SG_FLT_EPSILON)
	{
		onTheGround_ = true;
		return;
	}

	onTheGround_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitActivity_Goblin::UpdateGroundHitState(float _dt)
{
	PhysicsComponent* pPhysicsComponent = pActor_->GetComponent<PhysicsComponent>();

	if (pPhysicsComponent->HasForceX())
	{
		return;
	}

	// TODO: 죽음 확인 후 사망처리
	Stop();
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitActivity_Goblin::UpdateAirHitState(float _dt)
{
	if (!pActor_->IsOnTheGround())
	{
		return;
	}

	// TODO: 죽음 확인 후 사망처리

	if (!downTimeCheckBegin_)
	{
		pActor_->RunAnimation(DEF_ANIMATION_MONSTER_GOBLIN_FALL_DOWN_END);
		downTimeCheckBegin_ = true;
		return;
	}

	UpdateDownState(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitActivity_Goblin::UpdateDownState(float _dt)
{
	elapsedDownTime_ += _dt;

	if (elapsedDownTime_ >= downRecoverTime_)
	{
		AIComponent* pAIComponent = pActor_->GetComponent<AIComponent>();

		if (pAIComponent)
		{
			pAIComponent->RunActivity(AIActivityType::Sit);
		}
	}
}
