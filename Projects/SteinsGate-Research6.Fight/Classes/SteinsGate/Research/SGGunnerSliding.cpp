/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */


#include "SGGunnerSliding.h"

#include <SteinsGate/Research/SGPlayer.h>
#include <SteinsGate/Research/SGAnimationDefine.h>
#include <SteinsGate/Research/SGProjectileDefine.h>

SGGunnerSliding::SGGunnerSliding(SGPlayer* player, SGActionInfo* actionInfo)
	: SGGunnerAction(player, actionInfo)
{
	initHitRecorder(
		CC_CALLBACK_1(SGGunnerSliding::onEnemySingleHit, this),
		CC_CALLBACK_2(SGGunnerSliding::onEnemyMultiHit, this)
	);
}


void SGGunnerSliding::onActionBegin() {
	m_bSlidingStarted = false;
	m_pPlayer->runAnimation(GUNNER_ANIMATION_SLIDING);

	m_pHitRecorder->clear();
	m_pHitRecorder->setAlreadyHitRecord(true);
}

void SGGunnerSliding::onUpdate(float dt) {
	if (!m_bSlidingStarted)
		return;

	SGCharacter* pCharacter = m_pPlayer->getCharacter();

	if (pCharacter->hasForceX())
		return;

	// 더이상 X축 힘이 존재하지 않는 경우 중지
	stop();
}



void SGGunnerSliding::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {
}

void SGGunnerSliding::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {
}

void SGGunnerSliding::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {
	m_pHitRecorder->record(animation);
}

void SGGunnerSliding::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {
	SGCharacter* pCharacter = m_pPlayer->getCharacter();
	SpriteDirection_t eDir = pCharacter->getSpriteDirection();

	// 거너 특유의 슬라이딩 시작전 경직 효과를 주기위해 111번 프레임 끝난 후
	if (animation->getFrameIndex() == 111) {
		m_bSlidingStarted = true;
		if (eDir == SpriteDirection::Right)
			pCharacter->addForceX(pCharacter->getBaseInfo()->SlidingForce);
		else
			pCharacter->addForceX(-pCharacter->getBaseInfo()->SlidingForce);
	}

	// 일시정지 프레임 만나면 1번부터 다시 재시작
	if (animation->getFrameIndex() == 114 && animation->isZeroFramePaused()) {
		m_pPlayer->getCharacter()->runAnimation(GUNNER_ANIMATION_SLIDING, 1);
	}
}



void SGGunnerSliding::onEnemySingleHit(SGHitInfo& info) {
	if (m_pHitRecorder->isAlreadyHit(info.HitTarget))
		return;

	info.HitTarget->hit(info);
}


void SGGunnerSliding::onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount) {
	if (newHitCount > 0) {
		m_pPlayer->getCharacter()->pauseAnimation(FPS6_v);
		m_pPlayer->getCharacter()->pause(FPS6_v);
	}
}