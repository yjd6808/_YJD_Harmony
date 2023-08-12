/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:18:42 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/ProjectileListener.h>
#include <SteinsGate/Client/HitRecorder.h>

class ProjectileListener_GunnerBullet : public ProjectileListener
{
public:
	ProjectileListener_GunnerBullet(Projectile* projectile, Actor* spawner);

	void onCreated() override;
	void onUpdate(float dt) override;
	void onCollisionWithGround() override;
	void onEnemySingleHit(HitInfo& info);
	void onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount);

	SG_PROJECTILE_LISTENER_FACTORY(ProjectileListener_GunnerBullet)
};


