/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGGunnerSliding.h"

#include <SteinsGate/Research/SGHostPlayer.h>
#include <SteinsGate/Research/SGActionDefine.h>
#include <SteinsGate/Research/SGAnimationDefine.h>
#include <SteinsGate/Research/SGEffectDefine.h>
#include <SteinsGate/Research/SGActorBox.h>

SGGunnerSliding::SGGunnerSliding(SGHostPlayer* player, SGActionInfo* actionInfo)
	: SGGunnerAction(player, actionInfo)
{
}

bool SGGunnerSliding::onConditionCheck() {
	return m_pPlayer->getRunningActionCode() == GUNNER_ACTION_RUN;
}

void SGGunnerSliding::onActionBegin() {
	m_bSlidingStarted = false;
	m_pPlayer->runAnimation(GUNNER_ANIMATION_SLIDING);
	m_pHitRecorder->setAlreadyHitRecord(true);
	m_pHitRecorder->setSingleHitCallback(CC_CALLBACK_1(SGGunnerSliding::onEnemySingleHit, this));
	m_pHitRecorder->setMultiHitCallback(CC_CALLBACK_2(SGGunnerSliding::onEnemyMultiHit, this));
	
}

void SGGunnerSliding::onUpdate(float dt) {

	SGCharacter* pCharacter = m_pPlayer->getCharacter();
	SGActorPartAnimation* pAnimation = pCharacter->getRunningAnimation();
	m_pHitRecorder->record(pAnimation);

	if (!m_bSlidingStarted)
		return;

	if (pCharacter->hasForceX())
		return;

	// 더이상 X축 힘이 존재하지 않는 경우 중지
	stop();
}


void SGGunnerSliding::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {
	SGCharacter* pCharacter = m_pPlayer->getCharacter();
	SpriteDirection_t eDir = pCharacter->getSpriteDirection();

	// 거너 특유의 슬라이딩 시작전 경직 효과를 주기위해 111번 프레임 끝난 후
	if (animation->getFrameIndex() == 111) {
		m_bSlidingStarted = true;

		SGActorBox::get()->createEffectOnMapBySpawner(pCharacter, EFFECT_GUNNER_SLIDING_BEGIN, 250, 140);

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

	SGActorBox::get()->createEffectOnMapTargetCollision(EFFECT_KNOCK_BIG, info, true);
	info.HitTarget->hit(info);
}


void SGGunnerSliding::onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount) {
	if (newHitCount > 0) {
		m_pPlayer->getCharacter()->stiffenBody(FPS6_v);
	}
}