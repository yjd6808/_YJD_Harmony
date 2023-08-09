/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/IActorListener.h>
#include <SteinsGate/Client/Projectile.h>

class JCORE_NOVTABLE ProjectileListener : public IActorListener
{
public:
	ProjectileListener();

	void setActor(Actor* actor) override;
	void setSpawner(Actor* spawner);

	Actor* getActor() override { return m_pProjectile; }
	Type getListenerType() const override { return eProjectile; }

	void onCreated() override;
	void onCleanUp() override;
	void onUpdate(float dt) override;
	
	virtual void onCollisionWithGround();
	virtual void onLifeTimeOver();
	virtual void onDistanceOver();

	bool isLifeTimeOver() const { return m_fElapsedLifeTime >= m_pProjectile->getBaseInfo()->LifeTime; }
	bool isDistanceOver() const { return m_fMoveDistance >= m_pProjectile->getBaseInfo()->Distance; }
protected:
	float m_fMoveDistance;
	float m_fElapsedLifeTime;

	Projectile* m_pProjectile;
	Actor* m_pSpawner;
};


