/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:18:42 AM
 * =====================
 *
 */

#pragma once

#include "sgcl/ProjectileListener.h"
#include "sgcl/HitRecorder.h"

class ProjectileListener_GunnerBullet : public ProjectileListener
{
public:
	ProjectileListener_GunnerBullet(Projectile* _pProjectile, Actor* _pSpawner);

	void OnCreated() override;
	void OnUpdate(float _dt) override;
	void OnCollisionWithGround() override;
	void onEnemySingleHit(HitInfo& _info);
	void onEnemyMultiHit(SGHitInfoList& _hitList, int _newHitCount);

	SG_PROJECTILE_LISTENER_FACTORY(ProjectileListener_GunnerBullet)
};
