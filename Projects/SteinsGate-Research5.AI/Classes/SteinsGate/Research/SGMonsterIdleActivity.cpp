/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */


#include "SGMonsterIdleActivity.h"

#include <SteinsGate/Research/SGAIActor.h>
#include <SteinsGate/Research/SGAnimationDefine.h>


SGMonsterIdleActivity::SGMonsterIdleActivity(SGAIActor* actor) : SGAITimedActivity(actor, AIActivity::Idle) {}

void SGMonsterIdleActivity::onActivityBegin() {
	SGAITimedActivity::onActivityBegin();

	m_pActor->runAnimation(MONSTER_ANIMATION_IDLE);
}

void SGMonsterIdleActivity::onActivityEnd() {
	
}

