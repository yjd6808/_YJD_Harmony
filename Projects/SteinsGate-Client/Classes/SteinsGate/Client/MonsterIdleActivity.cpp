/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "MonsterIdleActivity.h"

#include <SteinsGate/Client/Monster.h>
#include <SteinsGate/Client/AnimationDefine.h>


MonsterIdleActivity::MonsterIdleActivity(Monster* monster)
	: MonsterActivity(monster, AIActivityType::Idle) {}

void MonsterIdleActivity::onUpdate(float dt) {
	updateLimitTime(dt);
}

void MonsterIdleActivity::onActivityBegin() {
	m_pMonster->runAnimation(MONSTER_ANIMATION_IDLE);
}

void MonsterIdleActivity::onActivityEnd() {
	
}

