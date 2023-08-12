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
	Projectile(ProjectileInfo* baseInfo);
	~Projectile() override;

	static Projectile* create(ProjectileInfo* baseInfo, Actor* spawner);

	void initialize() override;
	void initActorSprite() override;
	void initThicknessBox(const ThicknessBox& thicknessBox) override;
	void initPosition();
	void initListeners() override;
	void initComponents() override;

	EffectInfo* getSpawnEffectInfo();
	EffectInfo* getHitEffectInfo();

	void setSpawner(Actor* spawner);
	Actor* getSpawner();
	ProjectileInfo* getBaseInfo();
	ActorType_t getType() const override { return ActorType::Projectile; }
	int getCode() override { return m_pBaseInfo->Code; }
	int getListenerCode() const { return m_pBaseInfo->ListenerCode; }
private:
	JCORE_NOT_NULL ProjectileInfo* m_pBaseInfo;
	JCORE_NULLABLE Actor* m_pSpawner;
};