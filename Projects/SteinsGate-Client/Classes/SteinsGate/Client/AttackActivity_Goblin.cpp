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

AttackActivity_Goblin::AttackActivity_Goblin(Actor* actor) : AttackActivity(actor) {}

void AttackActivity_Goblin::onActivitySelectFromAIRoutine(AIInfo* aiInfo, AIState_t aiState) {}

void AttackActivity_Goblin::onActivityBegin() {
	m_pHitRecorder = m_pActor->getHitRecorder();
	m_pActor->runAnimation(DEF_ANIMATION_MONSTER_GOBLIN_ATTACK);

	if (m_pHitRecorder == nullptr) {
		return;
	}

	m_pHitRecorder->setRecord(true);
	m_pHitRecorder->setAlreadyHitRecord(true);
	m_pHitRecorder->clearAlreadyHitEnemies();
	m_pHitRecorder->addSingleHitCallback(DEF_EVENT_HIT_GOBLIN_ATTACK,  CC_CALLBACK_1(AttackActivity_Goblin::onEnemySingleHit, this));
	m_pHitRecorder->addMultiHitCallback(DEF_EVENT_HIT_GOBLIN_ATTACK, CC_CALLBACK_2(AttackActivity_Goblin::onEnemyMultiHit, this));
}

void AttackActivity_Goblin::onUpdate(float dt) {
}

void AttackActivity_Goblin::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* frame) {
	if (animation->getAnimationCode() == DEF_ANIMATION_MONSTER_GOBLIN_ATTACK)
		stop();
	
}


void AttackActivity_Goblin::onEnemySingleHit(HitInfo& info) {
	if (m_pHitRecorder->isAlreadyHit(info.HitTarget))
		return;

	ActorManager::Get()->createEffectOnMapTargetCollision(DEF_EFFECT_KNOCK_BIG, info, true);
	info.HitTarget->hit(info);
}


void AttackActivity_Goblin::onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount) {
	PhysicsComponent* pPhysicsComponent = m_pActor->getComponent<PhysicsComponent>();

	if (pPhysicsComponent && newHitCount > 0) {
		pPhysicsComponent->stiffenBody(Const::FPS::_6);
	}
}