/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGMonsterIdleActivity.h"

#include <SteinsGate/Research/SGMonster.h>
#include <SteinsGate/Research/SGAnimationDefine.h>


SGMonsterIdleActivity::SGMonsterIdleActivity(SGMonster* monster)
	: SGMonsterActivity(monster, AIActivity::Idle) {}

void SGMonsterIdleActivity::onUpdate(float dt) {
	updateLimitTime(dt);
}

void SGMonsterIdleActivity::onActivityBegin() {
	m_pMonster->runAnimation(MONSTER_ANIMATION_IDLE);
}

void SGMonsterIdleActivity::onActivityEnd() {
	
}

