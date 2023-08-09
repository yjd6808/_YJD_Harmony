/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:18:42 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/ProjectileListener.h>
#include <SteinsGate/Client/HitRecorder.h>

class HostPlayer;
class HitRecorder;
class GunnerBulletListener : public ProjectileListener
{
public:
	void onCreated() override;
	void onUpdate(float dt) override;
	void onCollisionWithGround() override;
	void onEnemySingleHit(HitInfo& info);
	void onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount);

	SG_ACTOR_LISTENER_CREATE(GunnerBulletListener)
};


