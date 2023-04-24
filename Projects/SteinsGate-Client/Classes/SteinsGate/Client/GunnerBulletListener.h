/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:18:42 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/CharProjectileListener.h>

#include "HitRecorder.h"

class HostPlayer;
class HitRecorder;
class GunnerBulletListener : public CharProjectileListener
{
public:
	void onCreated() override;
	void onUpdate(float dt) override;
	void onLifeTimeOver() override;
	void onDistanceOver() override;
	void onCollisionWithGround() override;
	void onEnemySingleHit(SGHitInfo& info);
	void onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount);

	CreateNew(GunnerBulletListener)
};


