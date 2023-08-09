/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "MonsterFallDownActivity.h"

#include <SteinsGate/Client/Monster.h>
#include <SteinsGate/Client/AnimationDefine.h>
#include <SteinsGate/Client/PhysicsComponent.h>

MonsterFallDownActivity::MonsterFallDownActivity(Monster* monster)
	: MonsterActivity(monster, AIActivityType::FallDown) {}

void MonsterFallDownActivity::onActivityBegin() {
	const MonsterStatInfo* pStatInfo = m_pMonster->getStatInfo();
	PhysicsComponent* pPhysicsComponent = m_pMonster->getComponent<PhysicsComponent>();

	m_pMonster->runAnimation(MONSTER_ANIMATION_FALL_DOWN_BEGIN);
	m_fElapsedDownTime = 0.0f;
	m_fDownRecoverTime = pStatInfo ? pStatInfo->DownRecoverTime / 2 : 1.0f;
	m_bBounced = false;
	m_bDown = false;

	if (pPhysicsComponent)
		pPhysicsComponent->enableElasticity();
}

void MonsterFallDownActivity::onActivityEnd() {
	PhysicsComponent* pPhysicsComponent = m_pMonster->getComponent<PhysicsComponent>();

	if (pPhysicsComponent)
		pPhysicsComponent->disableElasticity();
}

void MonsterFallDownActivity::onUpdate(float dt) {

	PhysicsComponent* pPhysicsComponent = m_pMonster->getComponent<PhysicsComponent>();

	// Step 1. 바닥에 충돌해서 공중으로 튀어올랐는지 확인
	if (pPhysicsComponent && pPhysicsComponent->isBounced() && !m_bBounced) {
		m_bBounced = true;
		m_pMonster->runAnimation(MONSTER_ANIMATION_FALL_DOWN_BOUNCE);
		return;
	}

	// Step 2. 공중으로 튀어올랐다가 다시 바닥에 닿았는지 확인
	if (!m_bDown && m_bBounced && m_pMonster->isOnTheGround()) {
		m_pMonster->runAnimation(MONSTER_ANIMATION_FALL_DOWN_END);
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
		m_pMonster->runActivity(AIActivityType::Sit);
	}
}


