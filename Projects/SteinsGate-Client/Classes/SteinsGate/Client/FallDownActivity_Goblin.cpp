/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "FallDownActivity_Goblin.h"

#include <SteinsGate/Client/Monster.h>
#include <SteinsGate/Client/Define_Animation.h>

#include <SteinsGate/Client/PhysicsComponent.h>
#include <SteinsGate/Client/AIComponent.h>

FallDownActivity_Goblin::FallDownActivity_Goblin(Actor* actor)
	: FallDownActivity(actor)
	, m_bBounced(false)
	, m_bDown(false)
	, m_fElapsedDownTime(0.0f)
	, m_fDownRecoverTime(0.0f)
{}

void FallDownActivity_Goblin::onActivityBegin() {
	Monster* pMonster = dynamic_cast<Monster*>(m_pActor);
	DebugAssert(pMonster);

	const MonsterStatInfo* pStatInfo = pMonster->getStatInfo();
	PhysicsComponent* pPhysicsComponent = pMonster->getComponent<PhysicsComponent>();

	pMonster->runAnimation(DEF_ANIMATION_MONSTER_GOBLIN_FALL_DOWN_BEGIN);
	m_fElapsedDownTime = 0.0f;
	m_fDownRecoverTime = pStatInfo ? pStatInfo->DownRecoverTime / 2 : 1.0f;
	m_bBounced = false;
	m_bDown = false;

	if (pPhysicsComponent)
		pPhysicsComponent->enableElasticity();
}

void FallDownActivity_Goblin::onActivityEnd() {
	PhysicsComponent* pPhysicsComponent = m_pActor->getComponent<PhysicsComponent>();

	if (pPhysicsComponent)
		pPhysicsComponent->disableElasticity();
}

void FallDownActivity_Goblin::onUpdate(float dt) {

	PhysicsComponent* pPhysicsComponent = m_pActor->getComponent<PhysicsComponent>();

	// Step 1. 바닥에 충돌해서 공중으로 튀어올랐는지 확인
	if (pPhysicsComponent && pPhysicsComponent->isBounced() && !m_bBounced) {
		m_bBounced = true;
		m_pActor->runAnimation(DEF_ANIMATION_MONSTER_GOBLIN_FALL_DOWN_BOUNCE);
		return;
	}

	// Step 2. 공중으로 튀어올랐다가 다시 바닥에 닿았는지 확인
	if (!m_bDown && m_bBounced && m_pActor->isOnTheGround()) {
		m_pActor->runAnimation(DEF_ANIMATION_MONSTER_GOBLIN_FALL_DOWN_END);
		m_bDown = true;
		return;
	}

	// Step 3. 바닥에 누워있는 시간 경과 체크
	if (!m_bDown) {
		return;
	}


	// if (죽어있는 경우)
	//   사망처리

	m_fElapsedDownTime += dt;

	if (m_fElapsedDownTime >= m_fDownRecoverTime) {

		AIComponent* pAIComponent = m_pActor->getComponent<AIComponent>();

		if (pAIComponent)
			pAIComponent->runActivity(AIActivityType::Sit);
	}
}


