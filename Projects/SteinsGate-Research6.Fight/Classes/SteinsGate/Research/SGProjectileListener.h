/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGActorListener.h>
#include <SteinsGate/Research/SGStruct.h>

class SGProjectile;
class SGProjectileListener : public SGActorListener
{
public:
	SGProjectileListener() : SGActorListener(ActorType::Projectile) {}

	void injectActor(SGActor* actor) override;

	virtual void onCollisionWithGround() {}
	virtual void onLifeTimeOver() {}
	virtual void onDistanceOver() {}
protected:
	SGProjectile* m_pProjectile{};
};


