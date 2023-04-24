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

	bool initVariables() override;
	void initActorSprite() override;
	void initThicknessBox(const ThicknessBox& thicknessBox) override;
	void initListener(ActorListener* listener) override;
	void initPosition();

	void update(float dt) override;
	void updateListenerEvent(float dt);

	void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) override;

	EffectInfo* getSpawnEffectInfo();
	EffectInfo* getHitEffectInfo();

	bool isLifeTimeOver();
	bool isDistanceOver();
	void setSpawner(Actor* spawner);

	Actor* getSpawner();
	ProjectileInfo* getBaseInfo();
private:
	float m_fMoveDistance;
	float m_fElapsedLifeTime;

	Actor* m_pSpawner;
	ProjectileInfo* m_pBaseInfo;
};