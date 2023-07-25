/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/ActorListener.h>
#include <SteinsGate/Client/Struct.h>

#include "Projectile.h"

class Projectile;
class JCORE_NOVTABLE ProjectileListener : public ActorListener
{
public:
	ProjectileListener();

	void injectActor(Actor* actor) override;

	void onCreated() override;
	void onUpdate(float dt) override;
	Type getListenerType() const override { return eProjectile; }

	virtual void onCollisionWithGround();
	virtual void onLifeTimeOver();
	virtual void onDistanceOver();

	bool isLifeTimeOver() const { return m_fElapsedLifeTime >= m_pProjectile->getBaseInfo()->LifeTime; }
	bool isDistanceOver() const { return m_fMoveDistance >= m_pProjectile->getBaseInfo()->Distance; }
protected:
	float m_fMoveDistance;
	float m_fElapsedLifeTime;

	Projectile* m_pProjectile;
};


