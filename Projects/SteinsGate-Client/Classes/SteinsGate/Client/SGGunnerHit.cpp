/*
 * 작성자: 윤정도
 * 생성일: 1/27/2023 8:03:49 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGGunnerHit.h"

#include <SteinsGate/Client/SGHostPlayer.h>
#include <SteinsGate/Client/SGAnimationDefine.h>

SGGunnerHit::SGGunnerHit(SGHostPlayer* player, SGActionInfo* actionInfo)
	: SGGunnerAction(player, actionInfo)
	, m_bHitSmall(false)
	, m_bOnTheGround(false)		// 초기 Hit 판정시 공중이었는지 아니면 바닥이었는지
{
}

void SGGunnerHit::onActionBegin() {
	m_pPlayer->enableElasticity();
	m_fElapsedDownTime = 0.0f;
	m_bDownTimeCheckBegin = false;
	m_fDownRecoverTime = m_pBaseInfo->DownRecoverTime / 2.0f;

	selectHitAnimation();
	checkPosition();

}

void SGGunnerHit::onActionEnd() { 
	m_pPlayer->disableElasticity();
}

void SGGunnerHit::onUpdate(float dt) {
	if (m_bOnTheGround) {
		updateGroundHitState(dt);
		return;
	}

	updateAirHitState(dt);
}

void SGGunnerHit::selectHitAnimation() {

	if (m_bHitSmall) {
		m_pPlayer->runAnimation(GUNNER_ANIMATION_HIT_SMALL);
	} else {
		m_pPlayer->runAnimation(GUNNER_ANIMATION_HIT_BIG);
	}
	m_bHitSmall = !m_bHitSmall;
}
void SGGunnerHit::checkPosition() {
	if (!m_pPlayer->hasForceY()) {
		m_bOnTheGround = true;
		return;
	}

	m_bOnTheGround = false;
}


void SGGunnerHit::updateGroundHitState(float dt) {
	if (m_pPlayer->hasForceX())
		return;

	// TODO: 죽음 확인 후 사망처리
	stop();
}

void SGGunnerHit::updateAirHitState(float dt) {
	if (!m_pPlayer->isOnTheGround())
		return;

	// TODO: 죽음 확인 후 사망처리

	if (!m_bDownTimeCheckBegin) {
		m_pPlayer->runAnimation(GUNNER_ANIMATION_FALL_DOWN_END);
		m_bDownTimeCheckBegin = true;
		return;
	}

	updateDownState(dt);
}

void SGGunnerHit::updateDownState(float dt) {
	m_fElapsedDownTime += dt;

	if (m_fElapsedDownTime >= m_fDownRecoverTime) {
		SGActionManager* pActionManager = m_pPlayer->actionManager();

		pActionManager->stopActionForce();
		pActionManager->runBaseAction(BaseAction::SitRecover);
	}
}