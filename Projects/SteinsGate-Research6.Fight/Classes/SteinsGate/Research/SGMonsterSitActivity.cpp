/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 10:17:26 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "SGMonsterSitActivity.h"

#include <SteinsGate/Research/SGMonster.h>
#include <SteinsGate/Research/SGAnimationDefine.h>


SGMonsterSitActivity::SGMonsterSitActivity(SGMonster* monster)
	: SGMonsterActivity(monster, AIActivity::Idle) {}

void SGMonsterSitActivity::onActivityBegin() {
	setLimit(m_pMonster->getBaseInfo()->DownRecoverTime / 2);
	m_pMonster->runAnimation(MONSTER_ANIMATION_SIT);
}

void SGMonsterSitActivity::onActivityEnd() {

}

void SGMonsterSitActivity::onUpdate(float dt) {
	updateLimitTime(dt);
}

