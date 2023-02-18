/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 10:33:18 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGGunnerFallDown.h"

#include <SteinsGate/Client/SGHostPlayer.h>
#include <SteinsGate/Client/SGAnimationDefine.h>

SGGunnerFallDown::SGGunnerFallDown(SGHostPlayer* player, SGActionInfo* actionInfo)
	: SGGunnerAction(player, actionInfo)
	, m_bBounced(false)
	, m_bDown(false)
{
}

void SGGunnerFallDown::onActionBegin() {
	
	m_pPlayer->runAnimation(GUNNER_ANIMATION_FALL_DOWN_BEGIN);
	m_pPlayer->enableElasticity();

	m_fElapsedDownTime = 0.0f;
	m_fDownRecoverTime = m_pBaseInfo->DownRecoverTime / 2.0f;
	m_bBounced = false;
	m_bDown = false;
}

void SGGunnerFallDown::onActionEnd() {
	m_pPlayer->disableElasticity();
}

void SGGunnerFallDown::onUpdate(float dt) {
	SGCharacter* pCharacter = m_pPlayer;


	// Step 1. 바닥에 충돌해서 공중으로 튀어올랐는지 확인
	if (pCharacter->isBounced() && !m_bBounced) {
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
		SGActionManager* pActionManager = m_pPlayer->actionManager();

		pActionManager->stopActionForce();
		pActionManager->runBaseAction(BaseAction::SitRecover);
	}
}

