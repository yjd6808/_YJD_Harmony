/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "AttackActivity_Goblin.h"

#include "sgcl/_Struct/CL_Character.h"

#include "sgcl/Monster.h"
#include "sgcl/ActorManager.h"
#include "sgcl/PhysicsComponent.h"
#include "sgcl/Define_Animation.h"
#include "sgcl/Define_Event.h"
#include "sgcl/Define_Effect.h"

//////////////////////////////////////////////////////////////////////////////////////////
AttackActivity_Goblin::AttackActivity_Goblin(Actor* _pActor)
: AttackActivity(_pActor)
{
}

void AttackActivity_Goblin::OnActivitySelectFromAiRoutine(AIInfo* _pAiInfo, AIState_t _aiState)
{
}

void AttackActivity_Goblin::OnActivityBegin()
{
	pHitRecorder_ = pActor_->GetHitRecorder();
	pActor_->RunAnimation(DEF_ANIMATION_MONSTER_GOBLIN_ATTACK);

	if (pHitRecorder_ == nullptr)
	{
		return;
	}

	pHitRecorder_->SetRecord(true);
	pHitRecorder_->SetAlreadyHitRecord(true);
	pHitRecorder_->ClearAlreadyHitEnemies();
	pHitRecorder_->AddSingleHitCallback(
		DEF_EVENT_HIT_GOBLIN_ATTACK, CC_CALLBACK_1(AttackActivity_Goblin::OnEnemySingleHit, this));
	pHitRecorder_->AddMultiHitCallback(
		DEF_EVENT_HIT_GOBLIN_ATTACK, CC_CALLBACK_2(AttackActivity_Goblin::OnEnemyMultiHit, this));
}

void AttackActivity_Goblin::OnUpdate(float _dt)
{
}

void AttackActivity_Goblin::OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
	if (_pAnimation->GetAnimationCode() == DEF_ANIMATION_MONSTER_GOBLIN_ATTACK)
	{
		Stop();
	}
}

void AttackActivity_Goblin::OnEnemySingleHit(HitInfo& _info)
{
	if (pHitRecorder_->IsAlreadyHit(_info.pHitTarget_))
	{
		return;
	}

	ActorManager::Get()->CreateEffectOnMapTargetCollision(DEF_EFFECT_KNOCK_BIG, _info, true);
	_info.pHitTarget_->Hit(_info);
}

void AttackActivity_Goblin::OnEnemyMultiHit(SGHitInfoList& _hitList, int _newHitCount)
{
	PhysicsComponent* pPhysicsComponent = pActor_->GetComponent<PhysicsComponent>();

	if (pPhysicsComponent && _newHitCount > 0)
	{
		pPhysicsComponent->StiffenBody(Const::FPS::_6);
	}
}
