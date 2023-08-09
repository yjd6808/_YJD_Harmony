/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 10:33:18 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GunnerFallDown.h"

#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/AnimationDefine.h>
#include <SteinsGate/Client/PhysicsComponent.h>

GunnerFallDown::GunnerFallDown(HostPlayer* player, ActionInfo* actionInfo)
	: GunnerAction(player, actionInfo)
	, m_bBounced(false)
	, m_bDown(false)
{
}

void GunnerFallDown::onActionBegin() {

	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	if (pPhysicsComponent)
		pPhysicsComponent->enableElasticity();

	m_pPlayer->runAnimation(GUNNER_ANIMATION_FALL_DOWN_BEGIN);
	m_fElapsedDownTime = 0.0f;
	m_fDownRecoverTime = m_pBaseInfo->DownRecoverTime / 2.0f;
	m_bBounced = false;
	m_bDown = false;
}

void GunnerFallDown::onActionEnd() {
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	if (pPhysicsComponent)
		pPhysicsComponent->disableElasticity();
}

void GunnerFallDown::onUpdate(float dt) {
	Character* pCharacter = m_pPlayer;
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();


	// Step 1. 바닥에 충돌해서 공중으로 튀어올랐는지 확인
	if (pPhysicsComponent && pPhysicsComponent->isBounced() && !m_bBounced) {
		m_bBounced = true;
		pCharacter->runAnimation(GUNNER_ANIMATION_FALL_DOWN_BOUNCE);
		return;
	}

	// Step 2. 공중으로 튀어올랐다가 다시 바닥에 닿았는지 확인
	if (!m_bDown && m_bBounced && pCharacter->isOnTheGround()) {
		pCharacter->runAnimation(GUNNER_ANIMATION_FALL_DOWN_END);
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
		ActionMgr* pActionManager = m_pPlayer->actionManager();

		pActionManager->stopActionForce();
		pActionManager->runBaseAction(BaseAction::SitRecover);
	}
}

