﻿/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:53:48 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ProjectileListener_GunnerBullet.h"

#include <SteinsGate/Client/Projectile.h>
#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/ActorPartAnimation.h>
#include <SteinsGate/Client/AttackDataInfo.h>
#include <SteinsGate/Client/Define_Effect.h>
#include <SteinsGate/Client/Define_Event.h>
#include <SteinsGate/Client/ActorBox.h>

ProjectileListener_GunnerBullet::ProjectileListener_GunnerBullet(Projectile* projectile, Actor* spawner)
	: ProjectileListener(projectile, spawner)
{}

void ProjectileListener_GunnerBullet::onCreated() {
	ProjectileListener::onCreated();
	HitRecorder* pHitRecorder = m_pProjectile->getHitRecorder();

	pHitRecorder->clearAlreadyHitEnemies();
	pHitRecorder->setAlreadyHitRecord(true);
	pHitRecorder->addSingleHitCallback(DEF_EVENT_HIT_GUNNER_BULLET, CC_CALLBACK_1(ProjectileListener_GunnerBullet::onEnemySingleHit, this));
	pHitRecorder->addMultiHitCallback(DEF_EVENT_HIT_GUNNER_BULLET, CC_CALLBACK_2(ProjectileListener_GunnerBullet::onEnemyMultiHit, this));
}

void ProjectileListener_GunnerBullet::onUpdate(float dt) {
	ProjectileListener::onUpdate(dt);

	const ActorRect& projectileActorRect = m_pProjectile->getActorRect();
	const int iAttackData = m_pProjectile->getBaseInfo()->AttackData->Code;

	m_pProjectile->getHitRecorder()->record(projectileActorRect, iAttackData);
}

void ProjectileListener_GunnerBullet::onCollisionWithGround() {
	ProjectileListener::onCollisionWithGround();
	ActorBox::Get()->createEffectOnMapAbsolute(
		DEF_EFFECT_COLLISION_FLOOR,
		m_pProjectile->getPositionRealCenterX(),
		m_pProjectile->getPositionRealCenterY(),
		m_pProjectile->getLocalZOrder() + 1
	);
}

void ProjectileListener_GunnerBullet::onEnemySingleHit(HitInfo& info) {
	if (m_pProjectile->getHitRecorder()->isAlreadyHit(info.HitTarget))
		return;

	EffectInfo* pHitEffectInfo = m_pProjectile->getHitEffectInfo();
	ActorBox::Get()->createEffectOnMapTargetCollision(
		pHitEffectInfo->Code,
		SpriteDirection::Reverse[info.HitDirection],
		info);
	info.HitTarget->hit(info);
	m_pProjectile->cleanUpAtNextFrame();
}


void ProjectileListener_GunnerBullet::onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount) {
}
