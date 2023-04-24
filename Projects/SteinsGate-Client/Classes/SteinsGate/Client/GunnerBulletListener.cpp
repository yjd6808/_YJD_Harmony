/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:53:48 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GunnerBulletListener.h"

#include <SteinsGate/Client/Projectile.h>
#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/ActorPartAnimation.h>
#include <SteinsGate/Client/AttackDataInfo.h>
#include <SteinsGate/Client/EffectDefine.h>
#include <SteinsGate/Client/ActorBox.h>

void GunnerBulletListener::onCreated() {
	HitRecorder* pHitRecorder = m_pProjectile->getHitRecorder();

	pHitRecorder->clear();
	pHitRecorder->setAlreadyHitRecord(true);
	pHitRecorder->setSingleHitCallback(CC_CALLBACK_1(GunnerBulletListener::onEnemySingleHit, this));
	pHitRecorder->setMultiHitCallback(CC_CALLBACK_2(GunnerBulletListener::onEnemyMultiHit, this));
}

void GunnerBulletListener::onUpdate(float dt) {
	const SGActorRect& projectileActorRect = m_pProjectile->getActorRect();
	const int iAttackData = m_pProjectile->getBaseInfo()->AttackData->Code;

	m_pProjectile->getHitRecorder()->record(projectileActorRect, iAttackData);
}

void GunnerBulletListener::onLifeTimeOver() {
	m_pProjectile->cleanUpNext();
}

void GunnerBulletListener::onDistanceOver() {
	m_pProjectile->cleanUpNext();
}

void GunnerBulletListener::onCollisionWithGround() {
	ActorBox::get()->createEffectOnMapAbsolute(
		EFFECT_COLLISION_FLOOR,
		m_pProjectile->getPositionRealCenterX(),
		m_pProjectile->getPositionRealCenterY(),
		m_pProjectile->getLocalZOrder() + 1
	);

	m_pProjectile->cleanUpNext();
}

void GunnerBulletListener::onEnemySingleHit(SGHitInfo& info) {
	if (m_pProjectile->getHitRecorder()->isAlreadyHit(info.HitTarget))
		return;

	EffectInfo* pHitEffectInfo = m_pProjectile->getHitEffectInfo();
	ActorBox::get()->createEffectOnMapTargetCollision(
		pHitEffectInfo->Code,
		SpriteDirection::Reverse[info.HitDirection],
		info);
	info.HitTarget->hit(info);
	m_pProjectile->cleanUpNext();
}


void GunnerBulletListener::onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount) {
}
