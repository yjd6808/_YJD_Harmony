/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:53:48 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGGunnerBulletListener.h"

#include <SteinsGate/Research/SGProjectile.h>
#include <SteinsGate/Research/SGPlayer.h>
#include <SteinsGate/Research/SGActorPartAnimation.h>
#include <SteinsGate/Research/SGAttackDataInfo.h>
#include <SteinsGate/Research/SGEffectDefine.h>
#include <SteinsGate/Research/SGActorBox.h>

void SGGunnerBulletListener::onCreated() {
	SGHitRecorder* pHitRecorder = m_pProjectile->getHitRecorder();

	pHitRecorder->clear();
	pHitRecorder->setAlreadyHitRecord(true);
	pHitRecorder->setSingleHitCallback(CC_CALLBACK_1(SGGunnerBulletListener::onEnemySingleHit, this));
	pHitRecorder->setMultiHitCallback(CC_CALLBACK_2(SGGunnerBulletListener::onEnemyMultiHit, this));
}

void SGGunnerBulletListener::onUpdate(float dt) {
	const SGActorRect& projectileActorRect = m_pProjectile->getActorRect();
	const int iAttackData = m_pProjectile->getBaseInfo()->AttackData->Code;

	m_pProjectile->getHitRecorder()->record(projectileActorRect, iAttackData);
}

void SGGunnerBulletListener::onLifeTimeOver() {
	m_pProjectile->registerCleanUp();
}

void SGGunnerBulletListener::onDistanceOver() {
	m_pProjectile->registerCleanUp();
}

void SGGunnerBulletListener::onCollisionWithGround() {
	SGActorBox::getInstance()->createEffectOnMapAbsolute(
		EFFECT_COLLISION_FLOOR,
		m_pProjectile->getPositionRealCenterX(),
		m_pProjectile->getPositionRealCenterY(),
		m_pProjectile->getLocalZOrder() + 1
	);

	m_pProjectile->registerCleanUp();
}

void SGGunnerBulletListener::onEnemySingleHit(SGHitInfo& info) {
	if (m_pProjectile->getHitRecorder()->isAlreadyHit(info.HitTarget))
		return;

	SGEffectInfo* pHitEffectInfo = m_pProjectile->getHitEffectInfo();
	SGActorBox::getInstance()->createEffectOnMapTargetCollision(
		pHitEffectInfo->Code,
		SpriteDirection::Reverse[info.HitDirection],
		info);
	info.HitTarget->hit(info);
	m_pProjectile->registerCleanUp();
}


void SGGunnerBulletListener::onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount) {
}
