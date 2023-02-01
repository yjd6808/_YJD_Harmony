/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */


#include "SGMonsterHitActivity.h"

#include <SteinsGate/Research/SGMonster.h>
#include <SteinsGate/Research/SGAnimationDefine.h>

// 공중에 떴을때 Hit 애니메이션 얼마나 실행시킬지
#define MinPlayHitAnimationOnTheAir	 0.3f


SGMonsterHitActivity::SGMonsterHitActivity(SGMonster* monster)
	: SGMonsterActivity(monster, AIActivity::Hit)
	, m_bHitSmall(true)
	, m_bOnTheGround(false) {}


void SGMonsterHitActivity::onActivityBegin() {
	m_fElapsedDownTime = 0.0f;
	m_bDownTimeCheckBegin = false;
	m_fDownRecoverTime = m_pMonster->getBaseInfo()->DownRecoverTime / 2;

	selectHitAnimation();
	checkPosition();
}


void SGMonsterHitActivity::onUpdate(float dt) {

	if (m_bOnTheGround) {
		updateGroundHitState(dt);
		return;
	}

	updateAirHitState(dt);
}

void SGMonsterHitActivity::selectHitAnimation() {
	if (m_bHitSmall) {
		m_pMonster->runAnimation(MONSTER_ANIMATION_HIT_SMALL);
	} else {
		m_pMonster->runAnimation(MONSTER_ANIMATION_HIT_BIG);
	}
	m_bHitSmall = !m_bHitSmall;
}

void SGMonsterHitActivity::checkPosition() {
	if (m_pMonster->isOnTheGround()) {
		m_bOnTheGround = true;
		return;
	}

	m_bOnTheGround = false;
}

void SGMonsterHitActivity::updateGroundHitState(float dt) {
	if (m_pMonster->hasForceX())
		return;

	// TODO: 죽음 확인 후 사망처리
	stop();
}

void SGMonsterHitActivity::updateAirHitState(float dt) {
	if (!m_pMonster->isOnTheGround())
		return;

	// TODO: 죽음 확인 후 사망처리

	if (!m_bDownTimeCheckBegin) {
		m_pMonster->runAnimation(MONSTER_ANIMATION_FALL_DOWN_END);
		m_bDownTimeCheckBegin = true;
		return;
	}

	updateDownState(dt);
}

void SGMonsterHitActivity::updateDownState(float dt) {
	m_fElapsedDownTime += dt;

	if (m_fElapsedDownTime >= m_fDownRecoverTime) {
		m_pMonster->runActivity(AIActivity::Sit);
	}
}
