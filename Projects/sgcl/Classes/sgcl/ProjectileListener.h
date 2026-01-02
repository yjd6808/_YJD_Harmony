/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */

#pragma once

#include <sgcl/IActorListener.h>
#include <sgcl/Projectile.h>

#define SG_PROJECTILE_LISTENER_FACTORY(Type)                                                \
public:                                                                                     \
    struct Factory : IFactory {                                                             \
        ProjectileListener* Create(Projectile* _pProjectile, Actor* _pSpawner) override {   \
            return dbg_new Type(_pProjectile, _pSpawner);                                   \
        }                                                                                   \
    };

class ProjectileListener : public IActorListener
{
public:
	struct IFactory
	{
		virtual ~IFactory() = default;
		virtual ProjectileListener* Create(Projectile* _pProjectile, Actor* _pSpawner) = 0;
	};

	ProjectileListener(Projectile* _pProjectile, Actor* _pSpawner);

	void OnCreated() override;
	void OnCleanUp() override;
	void OnUpdate(float _dt) override;

	virtual void OnCollisionWithGround();
	virtual void OnLifeTimeOver();
	virtual void OnDistanceOver();

	bool IsLifeTimeOver() const { return elapsedLifeTime_ >= pProjectile_->GetBaseInfo()->lifeTime_; }
	bool IsDistanceOver() const { return moveDistance_ >= pProjectile_->GetBaseInfo()->distance_; }

	Type GetListenerType() const override { return eProjectile; }

protected:
	NOT_NULL Projectile* pProjectile_;
	NULLABLE Actor* pSpawner_;

	float moveDistance_;
	float elapsedLifeTime_;
};
