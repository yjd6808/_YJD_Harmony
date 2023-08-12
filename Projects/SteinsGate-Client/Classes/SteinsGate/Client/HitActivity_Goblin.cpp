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


HitActivity_Goblin::HitActivity_Goblin(Actor* actor)
	: HitActivity(actor)
	, m_bHitSmall(true)
	, m_bOnTheGround(false)
	, m_bDownTimeCheckBegin(false)
	, m_fElapsedDownTime(0)
	, m_fDownRecoverTime(0)
{}


void HitActivity_Goblin::onActivityBegin() {
	Monster* pMonster = dynamic_cast<Monster*>(m_pActor);
	DebugAssert(pMonster);

	const MonsterStatInfo* pStatInfo = pMonster->getStatInfo();

	m_fElapsedDownTime = 0.0f;
	m_bDownTimeCheckBegin = false;
	m_fDownRecoverTime = pStatInfo ? pStatInfo->DownRecoverTime / 2 : 1.0f;

	selectHitAnimation();
	checkPosition();
}


void HitActivity_Goblin::onUpdate(float dt) {

	if (m_bOnTheGround) {
		updateGroundHitState(dt);
		return;
	}

	updateAirHitState(dt);
}

void HitActivity_Goblin::selectHitAnimation() {
	if (m_bHitSmall) {
		m_pActor->runAnimation(MONSTER_ANIMATION_GOBLIN_HIT_SMALL);
	} else {
		m_pActor->runAnimation(MONSTER_ANIMATION_GOBLIN_HIT_BIG);
	}
	m_bHitSmall = !m_bHitSmall;
}

void HitActivity_Goblin::checkPosition() {
	PhysicsComponent* pPhysicsComponent = m_pActor->getComponent<PhysicsComponent>();

	if (!pPhysicsComponent->hasForceY() && m_pActor->getPositionActorY() <= SG_FLT_EPSILON) {
		m_bOnTheGround = true;
		return;
	}

	m_bOnTheGround = false;
}

void HitActivity_Goblin::updateGroundHitState(float dt) {
	PhysicsComponent* pPhysicsComponent = m_pActor->getComponent<PhysicsComponent>();

	if (pPhysicsComponent->hasForceX()) {
		return;
	}

	// TODO: 죽음 확인 후 사망처리
	stop();
}

void HitActivity_Goblin::updateAirHitState(float dt) {


	if (!m_pActor->isOnTheGround())
		return;


	// TODO: 죽음 확인 후 사망처리

	if (!m_bDownTimeCheckBegin) {
		m_pActor->runAnimation(MONSTER_ANIMATION_GOBLIN_FALL_DOWN_END);
		m_bDownTimeCheckBegin = true;
		return;
	}

	updateDownState(dt);
}

void HitActivity_Goblin::updateDownState(float dt) {
	m_fElapsedDownTime += dt;

	if (m_fElapsedDownTime >= m_fDownRecoverTime) {
		AIComponent* pAIComponent = m_pActor->getComponent<AIComponent>();

		if (pAIComponent)
			pAIComponent->runActivity(AIActivityType::Sit);	
	}
}
