/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 10:17:26 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "MonsterSitActivity.h"

#include <SteinsGate/Client/Monster.h>
#include <SteinsGate/Client/AnimationDefine.h>


MonsterSitActivity::MonsterSitActivity(Monster* monster)
	: MonsterActivity(monster, AIActivityType::Idle) {}

void MonsterSitActivity::onActivityBegin() {
	setLimit(m_pMonster->getBaseInfo()->DownRecoverTime / 2);
	m_pMonster->runAnimation(MONSTER_ANIMATION_SIT);
}

void MonsterSitActivity::onActivityEnd() {

}

void MonsterSitActivity::onUpdate(float dt) {
	updateLimitTime(dt);
}

