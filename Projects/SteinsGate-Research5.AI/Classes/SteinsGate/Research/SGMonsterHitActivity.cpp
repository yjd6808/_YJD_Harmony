/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */


#include "SGMonsterHitActivity.h"

#include <SteinsGate/Research/SGAIActor.h>
#include <SteinsGate/Research/SGAnimationDefine.h>


SGMonsterHitActivity::SGMonsterHitActivity(SGAIActor* actor) : SGAIActivity(actor, AIActivity::Hit) {}

void SGMonsterHitActivity::onActivityBegin() {

	m_pActor->runAnimation(MONSTER_ANIMATION_HIT_SMALL);
}

void SGMonsterHitActivity::onUpdate(float dt) {
	
}



