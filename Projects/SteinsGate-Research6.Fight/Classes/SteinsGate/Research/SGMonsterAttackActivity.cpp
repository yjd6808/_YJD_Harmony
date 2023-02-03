/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGMonsterAttackActivity.h"

#include <SteinsGate/Research/SGMonster.h>
#include <SteinsGate/Research/SGAnimationDefine.h>


SGMonsterAttackActivity::SGMonsterAttackActivity(SGMonster* monster)
	: SGMonsterActivity(monster, AIActivity::Attack)
	, m_pHitRecorder(monster->getHitRecorder()){}

void SGMonsterAttackActivity::onActivityBegin() {
	m_pMonster->runAnimation(MONSTER_ANIMATION_ATTACK);

	m_pHitRecorder->setRecord(true);
	m_pHitRecorder->setSingleHitCallback(CC_CALLBACK_1(SGMonsterAttackActivity::onEnemySingleHit, this));
	m_pHitRecorder->setMultiHitCallback(CC_CALLBACK_2(SGMonsterAttackActivity::onEnemyMultiHit, this));
}

void SGMonsterAttackActivity::onUpdate(float dt) {
	SGActorPartAnimation* pAnimation = m_pMonster->getRunningAnimation();
	m_pHitRecorder->record(pAnimation);
}

void SGMonsterAttackActivity::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {
	if (animation->getAnimationCode() == MONSTER_ANIMATION_ATTACK)
		stop();
	
}


void SGMonsterAttackActivity::onEnemySingleHit(SGHitInfo& info) {
	if (m_pHitRecorder->isAlreadyHit(info.HitTarget))
		return;

	info.HitTarget->hit(info);
}


void SGMonsterAttackActivity::onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount) {
	if (newHitCount > 0) {
		m_pMonster->stiffenBody(FPS6_v);
	}
}