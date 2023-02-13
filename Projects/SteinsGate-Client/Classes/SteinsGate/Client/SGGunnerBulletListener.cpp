/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:53:48 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGGunnerBulletListener.h"

#include <SteinsGate/Client/SGProjectile.h>
#include <SteinsGate/Client/SGHostPlayer.h>
#include <SteinsGate/Client/SGActorPartAnimation.h>
#include <SteinsGate/Client/SGAttackDataInfo.h>
#include <SteinsGate/Client/SGEffectDefine.h>
#include <SteinsGate/Client/SGActorBox.h>

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
	m_pProjectile->cleanUpNext();
}

void SGGunnerBulletListener::onDistanceOver() {
	m_pProjectile->cleanUpNext();
}

void SGGunnerBulletListener::onCollisionWithGround() {
	SGActorBox::get()->createEffectOnMapAbsolute(
		EFFECT_COLLISION_FLOOR,
		m_pProjectile->getPositionRealCenterX(),
		m_pProjectile->getPositionRealCenterY(),
		m_pProjectile->getLocalZOrder() + 1
	);

	m_pProjectile->cleanUpNext();
}

void SGGunnerBulletListener::onEnemySingleHit(SGHitInfo& info) {
	if (m_pProjectile->getHitRecorder()->isAlreadyHit(info.HitTarget))
		return;

	SGEffectInfo* pHitEffectInfo = m_pProjectile->getHitEffectInfo();
	SGActorBox::get()->createEffectOnMapTargetCollision(
		pHitEffectInfo->Code,
		SpriteDirection::Reverse[info.HitDirection],
		info);
	info.HitTarget->hit(info);
	m_pProjectile->cleanUpNext();
}


void SGGunnerBulletListener::onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount) {
}
