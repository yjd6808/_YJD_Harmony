/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:20 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Actor.h>
#include <SteinsGate/Client/ProjectileInfo.h>

class Projectile : public Actor
{
public:
	Projectile(ProjectileInfo* _pBaseInfo);
	~Projectile() override;

	static Projectile* Create(ProjectileInfo* _pBaseInfo, Actor* _pSpawner);

	void Initialize() override;
	void InitActorSprite() override;
	void InitThicknessBox(const ThicknessBox& _thicknessBox) override;
	void InitPosition();
	void InitListeners() override;
	void InitComponents() override;

	EffectInfo*			GetSpawnEffectInfo();
	EffectInfo*			GetHitEffectInfo();

	void				SetSpawner(Actor* _pSpawner);
	Actor*				GetSpawner();
	ProjectileInfo*		GetBaseInfo();
	ActorType_t			GetType() const override { return ActorType::Projectile; }
	int					GetCode() override { return baseInfo_->code_; }
	int					GetListenerCode() const { return baseInfo_->listenerCode_; }

private:
	JCORE_NOT_NULL ProjectileInfo* baseInfo_;
	JCORE_NULLABLE Actor* spawner_;
};
