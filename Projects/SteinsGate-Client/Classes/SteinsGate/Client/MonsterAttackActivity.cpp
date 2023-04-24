/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "MonsterAttackActivity.h"

#include <SteinsGate/Client/Monster.h>
#include <SteinsGate/Client/AnimationDefine.h>
#include <SteinsGate/Client/ActorBox.h>
#include <SteinsGate/Client/EffectDefine.h>


MonsterAttackActivity::MonsterAttackActivity(Monster* monster)
	: MonsterActivity(monster, AIActivityType::Attack)
	, m_pHitRecorder(monster->getHitRecorder()){}

void MonsterAttackActivity::onActivityBegin() {
	m_pMonster->runAnimation(MONSTER_ANIMATION_ATTACK);

	m_pHitRecorder->clear();
	m_pHitRecorder->setRecord(true);
	m_pHitRecorder->setAlreadyHitRecord(true);
	m_pHitRecorder->setSingleHitCallback(CC_CALLBACK_1(MonsterAttackActivity::onEnemySingleHit, this));
	m_pHitRecorder->setMultiHitCallback(CC_CALLBACK_2(MonsterAttackActivity::onEnemyMultiHit, this));
}

void MonsterAttackActivity::onUpdate(float dt) {
	ActorPartAnimation* pAnimation = m_pMonster->getRunningAnimation();
	m_pHitRecorder->record(pAnimation);
}

void MonsterAttackActivity::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* frame) {
	if (animation->getAnimationCode() == MONSTER_ANIMATION_ATTACK)
		stop();
	
}


void MonsterAttackActivity::onEnemySingleHit(SGHitInfo& info) {
	if (m_pHitRecorder->isAlreadyHit(info.HitTarget))
		return;

	ActorBox::get()->createEffectOnMapTargetCollision(EFFECT_KNOCK_BIG, info, true);
	info.HitTarget->hit(info);
}


void MonsterAttackActivity::onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount) {
	if (newHitCount > 0) {
		m_pMonster->stiffenBody(FPS6_v);
	}
}