/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 3:38:32 PM
 * =====================
 *
 */


#include "SGActorPartAnimation.h"

#include <SteinsGate/Research/SGActorPartSprite.h>



SGActorPartAnimation* SGActorPartAnimation::create() {
	SGActorPartAnimation* pAnimation = new SGActorPartAnimation();

	if (pAnimation) {
		pAnimation->autorelease();
		return pAnimation;
	}

	DeleteSafe(pAnimation);
	return nullptr;
}

void SGActorPartAnimation::run() {}

void SGActorPartAnimation::update(float dt) {
	m_fRunningFrameTime += dt;

	if (m_fRunningFrameTime >= m_fLimitFrameTime) {
		m_fRunningFrameTime = 0;
		++m_iRunningFrameIndex;
		m_pTarget->setTexture(m_Frames[m_iRunningFrameIndex]->getTexture());
	}
}


