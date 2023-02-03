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

void SGGunnerBulletListener::onCreated() {
	SGHitRecorder* pHitRecorder = m_pProjectile->getHitRecorder();

	pHitRecorder->clear();
	pHitRecorder->setAlreadyHitRecord(true);
	pHitRecorder->setSingleHitCallback(CC_CALLBACK_1(SGGunnerBulletListener::onEnemySingleHit, this));
	pHitRecorder->setMultiHitCallback(CC_CALLBACK_2(SGGunnerBulletListener::onEnemyMultiHit, this));
}

void SGGunnerBulletListener::onUpdate(float dt) {
	m_pProjectile->getHitRecorder()->record(m_pProjectile->getRunningAnimation());
}

void SGGunnerBulletListener::onLifeTimeOver() {
	m_pProjectile->registerCleanUp();
}

void SGGunnerBulletListener::onDistanceOver() {
	m_pProjectile->registerCleanUp();
}

void SGGunnerBulletListener::onCollisionWithGround() {
	// TODO: 타격 이펙트 재생
	m_pProjectile->registerCleanUp();
}

void SGGunnerBulletListener::onEnemySingleHit(SGHitInfo& info) {
	if (m_pProjectile->getHitRecorder()->isAlreadyHit(info.HitTarget))
		return;

	// TODO: 타격 이펙트 재생
	info.HitTarget->hit(info);
}


void SGGunnerBulletListener::onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount) {
}
