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

	static Projectile* create(ProjectileInfo* _pBaseInfo, Actor* _pSpawner);

	void initialize() override;
	void initActorSprite() override;
	void initThicknessBox(const ThicknessBox& _thicknessBox) override;
	void initPosition();
	void initListeners() override;
	void initComponents() override;

	EffectInfo* getSpawnEffectInfo();
	EffectInfo* getHitEffectInfo();

	void setSpawner(Actor* _pSpawner);
	Actor* getSpawner();
	ProjectileInfo* getBaseInfo();
	ActorType_t getType() const override { return ActorType::Projectile; }
	int getCode() override { return baseInfo_->code_; }
	int getListenerCode() const { return baseInfo_->ListenerCode; }

private:
	JCORE_NOT_NULL ProjectileInfo* baseInfo_;
	JCORE_NULLABLE Actor* spawner_;
};
