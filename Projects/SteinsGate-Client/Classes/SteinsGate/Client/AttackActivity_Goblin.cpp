/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "AttackActivity_Goblin.h"

#include <SteinsGate/Client/Monster.h>
#include <SteinsGate/Client/ActorManager.h>
#include <SteinsGate/Client/PhysicsComponent.h>
#include <SteinsGate/Client/Define_Animation.h>
#include <SteinsGate/Client/Define_Event.h>
#include <SteinsGate/Client/Define_Effect.h>

//////////////////////////////////////////////////////////////////////////////////////////
AttackActivity_Goblin::AttackActivity_Goblin(Actor* _pActor)
	: AttackActivity(_pActor)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void AttackActivity_Goblin::onActivitySelectFromAIRoutine(AIInfo* _pAiInfo, AIState_t _aiState)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void AttackActivity_Goblin::onActivityBegin()
{
	m_pHitRecorder = m_pActor->getHitRecorder();
	m_pActor->runAnimation(DEF_ANIMATION_MONSTER_GOBLIN_ATTACK);

	if (m_pHitRecorder == nullptr)
	{
		return;
	}

	m_pHitRecorder->setRecord(true);
	m_pHitRecorder->setAlreadyHitRecord(true);
	m_pHitRecorder->clearAlreadyHitEnemies();
	m_pHitRecorder->addSingleHitCallback(DEF_EVENT_HIT_GOBLIN_ATTACK, CC_CALLBACK_1(AttackActivity_Goblin::onEnemySingleHit, this));
	m_pHitRecorder->addMultiHitCallback(DEF_EVENT_HIT_GOBLIN_ATTACK, CC_CALLBACK_2(AttackActivity_Goblin::onEnemyMultiHit, this));
}

//////////////////////////////////////////////////////////////////////////////////////////
void AttackActivity_Goblin::onUpdate(float _dt)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void AttackActivity_Goblin::onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
	if (_pAnimation->getAnimationCode() == DEF_ANIMATION_MONSTER_GOBLIN_ATTACK)
		stop();
}

//////////////////////////////////////////////////////////////////////////////////////////
void AttackActivity_Goblin::onEnemySingleHit(HitInfo& _info)
{
	if (m_pHitRecorder->isAlreadyHit(_info.HitTarget))
	{
		return;
	}

	ActorManager::Get()->createEffectOnMapTargetCollision(DEF_EFFECT_KNOCK_BIG, _info, true);
	_info.HitTarget->hit(_info);
}

//////////////////////////////////////////////////////////////////////////////////////////
void AttackActivity_Goblin::onEnemyMultiHit(SGHitInfoList& _hitList, int _newHitCount)
{
	PhysicsComponent* pPhysicsComponent = m_pActor->getComponent<PhysicsComponent>();

	if (pPhysicsComponent && _newHitCount > 0)
	{
		pPhysicsComponent->stiffenBody(Const::FPS::_6);
	}
}
