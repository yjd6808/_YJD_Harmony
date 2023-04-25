﻿/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "MonsterHitActivity.h"

#include <SteinsGate/Client/Monster.h>
#include <SteinsGate/Client/AnimationDefine.h>


MonsterHitActivity::MonsterHitActivity(Monster* monster)
	: MonsterActivity(monster, AIActivityType::Hit)
	, m_bHitSmall(true)
	, m_bOnTheGround(false) {}


void MonsterHitActivity::onActivityBegin() {
	m_fElapsedDownTime = 0.0f;
	m_bDownTimeCheckBegin = false;
	m_fDownRecoverTime = m_pMonster->getBaseInfo()->DownRecoverTime / 2;

	selectHitAnimation();
	checkPosition();
}


void MonsterHitActivity::onUpdate(float dt) {

	if (m_bOnTheGround) {
		updateGroundHitState(dt);
		return;
	}

	updateAirHitState(dt);
}

void MonsterHitActivity::selectHitAnimation() {
	if (m_bHitSmall) {
		m_pMonster->runAnimation(MONSTER_ANIMATION_HIT_SMALL);
	} else {
		m_pMonster->runAnimation(MONSTER_ANIMATION_HIT_BIG);
	}
	m_bHitSmall = !m_bHitSmall;
}

void MonsterHitActivity::checkPosition() {
	if (!m_pMonster->hasForceY()) {
		m_bOnTheGround = true;
		return;
	}

	m_bOnTheGround = false;
}

void MonsterHitActivity::updateGroundHitState(float dt) {
	if (m_pMonster->hasForceX())
		return;

	// TODO: 죽음 확인 후 사망처리
	stop();
}

void MonsterHitActivity::updateAirHitState(float dt) {


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

void MonsterHitActivity::updateDownState(float dt) {
	m_fElapsedDownTime += dt;

	if (m_fElapsedDownTime >= m_fDownRecoverTime) {
		m_pMonster->runActivity(AIActivityType::Sit);
	}
}