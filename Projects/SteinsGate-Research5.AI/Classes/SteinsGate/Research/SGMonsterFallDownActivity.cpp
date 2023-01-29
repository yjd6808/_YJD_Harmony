/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */


#include "SGMonsterFallDownActivity.h"

#include <SteinsGate/Research/SGAIActor.h>
#include <SteinsGate/Research/SGAnimationDefine.h>


SGMonsterFallDownActivity::SGMonsterFallDownActivity(SGAIActor* actor) : SGAIActivity(actor, AIActivity::FallDown) {}

void SGMonsterFallDownActivity::onActivityBegin() {
	SGMonsterFallDownActivity::onActivityBegin();

	// 공중에 있는 상태에서 사망하는 경우 넘어진 상태로 시작해야하는데
	// if (m_pActor->getPositionActorY() > 10.0f)

	m_pActor->runAnimation(MONSTER_ANIMATION_FALL_DOWN);
}

void SGMonsterFallDownActivity::onUpdate(float dt) {
	
}

void SGMonsterFallDownActivity::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {

}


