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
	Projectile(Actor* spawner, ProjectileInfo* baseInfo);
	~Projectile() override;

	static Projectile* create(Actor* spawner, ProjectileInfo* baseInfo);

	void initActorSprite() override;
	void initThicknessBox(const ThicknessBox& thicknessBox) override;
	void initPosition();

	EffectInfo* getSpawnEffectInfo();
	EffectInfo* getHitEffectInfo();

	void setSpawner(Actor* spawner);

	Actor* getSpawner();
	ProjectileInfo* getBaseInfo();
	int getCode() override { return m_pBaseInfo->Code; }
private:
	Actor* m_pSpawner;
	ProjectileInfo* m_pBaseInfo;
};