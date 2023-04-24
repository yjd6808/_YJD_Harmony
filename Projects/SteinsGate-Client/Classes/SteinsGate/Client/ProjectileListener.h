/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/ActorListener.h>
#include <SteinsGate/Client/Struct.h>

class Projectile;
class ProjectileListener : public ActorListener
{
public:
	ProjectileListener() : ActorListener(ActorType::Projectile) {}

	void injectActor(Actor* actor) override;

	virtual void onCollisionWithGround() {}
	virtual void onLifeTimeOver() {}
	virtual void onDistanceOver() {}
protected:
	Projectile* m_pProjectile{};
};


