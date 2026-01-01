/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Core.h"
#include "FallDownActivity_Goblin.h"

#include <sgcl/Monster.h>
#include <sgcl/Define_Animation.h>

#include <sgcl/PhysicsComponent.h>
#include <sgcl/AIComponent.h>

FallDownActivity_Goblin::FallDownActivity_Goblin(Actor* _pActor)
: FallDownActivity(_pActor)
, bounced_(false)
, down_(false)
, elapsedDownTime_(0.0f)
, downRecoverTime_(0.0f)
{
}

void FallDownActivity_Goblin::OnActivityBegin()
{
	Monster* pMonster = dynamic_cast<Monster*>(pActor_);
	jc_assert(pMonster);

	const MonsterStatInfo* pStatInfo = pMonster->GetStatInfo();
	PhysicsComponent* pPhysicsComponent = pMonster->GetComponent<PhysicsComponent>();

	pMonster->RunAnimation(DEF_ANIMATION_MONSTER_GOBLIN_FALL_DOWN_BEGIN);
	elapsedDownTime_ = 0.0f;
	downRecoverTime_ = pStatInfo ? pStatInfo->downRecoverTime_ / 2 : 1.0f;
	bounced_ = false;
	down_ = false;

	if (pPhysicsComponent)
		pPhysicsComponent->EnableElasticity();
}

void FallDownActivity_Goblin::OnActivityEnd()
{
	PhysicsComponent* pPhysicsComponent = pActor_->GetComponent<PhysicsComponent>();

	if (pPhysicsComponent)
		pPhysicsComponent->DisableElasticity();
}

void FallDownActivity_Goblin::OnUpdate(float _dt)
{
	PhysicsComponent* pPhysicsComponent = pActor_->GetComponent<PhysicsComponent>();

	// Step 1. 바닥에 충돌해서 공중으로 튀어올랐는지 확인
	if (pPhysicsComponent && pPhysicsComponent->IsBounced() && !bounced_)
	{
		bounced_ = true;
		pActor_->RunAnimation(DEF_ANIMATION_MONSTER_GOBLIN_FALL_DOWN_BOUNCE);
		return;
	}

	// Step 2. 공중으로 튀어올랐다가 다시 바닥에 닿았는지 확인
	if (!down_ && bounced_ && pActor_->IsOnTheGround())
	{
		pActor_->RunAnimation(DEF_ANIMATION_MONSTER_GOBLIN_FALL_DOWN_END);
		down_ = true;
		return;
	}

	// Step 3. 바닥에 누워있는 시간 경과 체크
	if (!down_)
	{
		return;
	}


	// if (죽어있는 경우)
	//   사망처리

	elapsedDownTime_ += _dt;

	if (elapsedDownTime_ >= downRecoverTime_)
	{
		AIComponent* pAIComponent = pActor_->GetComponent<AIComponent>();

		if (pAIComponent)
			pAIComponent->RunActivity(AIActivityType::Sit);
	}
}
