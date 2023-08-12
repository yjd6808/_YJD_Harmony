/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/IActorListener.h>
#include <SteinsGate/Client/Projectile.h>

#define SG_PROJECTILE_LISTENER_FACTORY(Type)												\
public:																						\
	struct Factory : IFactory {																\
		ProjectileListener* create(Projectile* projectile, Actor* spawner) override {		\
			return dbg_new Type(projectile, spawner);										\
		}																					\
	};

class ProjectileListener : public IActorListener
{
public:
	struct IFactory
	{
		virtual ~IFactory() = default;
		virtual ProjectileListener* create(Projectile* projectile, Actor* spawner) = 0;
	};

	ProjectileListener(Projectile* projectile, Actor* spawner);

	void onCreated() override;
	void onCleanUp() override;
	void onUpdate(float dt) override;
	
	virtual void onCollisionWithGround();
	virtual void onLifeTimeOver();
	virtual void onDistanceOver();

	bool isLifeTimeOver() const { return m_fElapsedLifeTime >= m_pProjectile->getBaseInfo()->LifeTime; }
	bool isDistanceOver() const { return m_fMoveDistance >= m_pProjectile->getBaseInfo()->Distance; }

	Type getListenerType() const override { return eProjectile; }
protected:
	JCORE_NOT_NULL Projectile* m_pProjectile;
	JCORE_NULLABLE Actor* m_pSpawner;

	float m_fMoveDistance;
	float m_fElapsedLifeTime;
};


