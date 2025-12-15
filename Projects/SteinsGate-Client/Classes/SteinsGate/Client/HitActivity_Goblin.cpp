/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "HitActivity_Goblin.h"

#include <SteinsGate/Client/Monster.h>
#include <SteinsGate/Client/Define_Animation.h>
#include <SteinsGate/Client/PhysicsComponent.h>
#include <SteinsGate/Client/AIComponent.h>

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
	DebugAssert(pMonster);

	const MonsterStatInfo* pStatInfo = pMonster->getStatInfo();

	elapsedDownTime_ = 0.0f;
	downTimeCheckBegin_ = false;
	downRecoverTime_ = pStatInfo ? pStatInfo->downRecoverTime_ / 2 : 1.0f;

	selectHitAnimation();
	checkPosition();
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitActivity_Goblin::OnUpdate(float _dt)
{
	if (onTheGround_)
	{
		updateGroundHitState(_dt);
		return;
	}

	updateAirHitState(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitActivity_Goblin::selectHitAnimation()
{
	if (hitSmall_)
	{
		pActor_->runAnimation(DEF_ANIMATION_MONSTER_GOBLIN_HIT_SMALL);
	}
	else
	{
		pActor_->runAnimation(DEF_ANIMATION_MONSTER_GOBLIN_HIT_BIG);
	}

	hitSmall_ = !hitSmall_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitActivity_Goblin::checkPosition()
{
	PhysicsComponent* pPhysicsComponent = pActor_->getComponent<PhysicsComponent>();

	if (!pPhysicsComponent->hasForceY() && pActor_->getPositionActorY() <= SG_FLT_EPSILON)
	{
		onTheGround_ = true;
		return;
	}

	onTheGround_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitActivity_Goblin::updateGroundHitState(float _dt)
{
	PhysicsComponent* pPhysicsComponent = pActor_->getComponent<PhysicsComponent>();

	if (pPhysicsComponent->hasForceX())
	{
		return;
	}

	// TODO: 죽음 확인 후 사망처리
	Stop();
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitActivity_Goblin::updateAirHitState(float _dt)
{
	if (!pActor_->isOnTheGround())
	{
		return;
	}

	// TODO: 죽음 확인 후 사망처리

	if (!downTimeCheckBegin_)
	{
		pActor_->runAnimation(DEF_ANIMATION_MONSTER_GOBLIN_FALL_DOWN_END);
		downTimeCheckBegin_ = true;
		return;
	}

	updateDownState(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HitActivity_Goblin::updateDownState(float _dt)
{
	elapsedDownTime_ += _dt;

	if (elapsedDownTime_ >= downRecoverTime_)
	{
		AIComponent* pAIComponent = pActor_->getComponent<AIComponent>();

		if (pAIComponent)
		{
			pAIComponent->runActivity(AIActivityType::Sit);
		}
	}
}
