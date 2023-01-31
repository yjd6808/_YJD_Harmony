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
	m_pActor->runAnimation(MONSTER_ANIMATION_FALL_DOWN_BEGIN);
	m_pActor->enableElasticity();
	m_bBounced = false;
	m_bDown = false;
}

void SGMonsterFallDownActivity::onActivityEnd() {
	m_pActor->disableElasticity();
}

void SGMonsterFallDownActivity::onUpdate(float dt) {

	// Step 1. 바닥에 충돌해서 공중으로 튀어올랐는지 확인
	if (m_pActor->isBounced() && !m_bBounced) {
		m_bBounced = true;
		m_pActor->runAnimation(MONSTER_ANIMATION_FALL_DOWN_BOUNCE);
		return;
	}

	// Step 2. 공중으로 튀어올랐다가 다시 바닥에 닿았는지 확인
	if (!m_bDown && m_bBounced && m_pActor->isOnTheGround()) {
		m_pActor->runAnimation(MONSTER_ANIMATION_FALL_DOWN_END);
		m_bDown = true;
		return;
	}

	// Step 3. 바닥에 누워있는 시간 경과 체크
	if (!m_bDown) {
		return;
	}

	m_fDownTime += dt;
}

void SGMonsterFallDownActivity::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {

}


