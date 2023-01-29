/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */


#include "SGMonsterAttackActivity.h"

#include <SteinsGate/Research/SGAIActor.h>
#include <SteinsGate/Research/SGAnimationDefine.h>


SGMonsterAttackActivity::SGMonsterAttackActivity(SGAIActor* actor) : SGAIActivity(actor, AIActivity::Attack) {}

void SGMonsterAttackActivity::onActivityBegin() {

	m_pActor->runAnimation(MONSTER_ANIMATION_ATTACK);
}

void SGMonsterAttackActivity::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {
	if (animation->getAnimationCode() == MONSTER_ANIMATION_ATTACK)
		stop();
	
}

void SGMonsterAttackActivity::onUpdate(float dt) {}
void SGMonsterAttackActivity::onActivityEnd() {}


