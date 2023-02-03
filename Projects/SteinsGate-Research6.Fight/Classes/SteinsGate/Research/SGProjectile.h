/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:20 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGActor.h>
#include <SteinsGate/Research/SGProjectileInfo.h>

class SGProjectile : public SGActor
{
public:
	SGProjectile(SGActor* spawner, SGProjectileInfo* baseInfo);
	~SGProjectile() override;

	static SGProjectile* create(SGActor* spawner, SGProjectileInfo* baseInfo);

	bool init() override;
	void initActorSprite() override;
	void initThicknessBox(const SGThicknessBox& thicknessBox) override;
	void initListener(SGActorListener* listener) override;

	void update(float dt) override;
	void updateListenerEvent(float dt);

	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;


	bool isLifeTimeOver();
	bool isDistanceOver();

	SGActor* getSpawner();
	SGProjectileInfo* getBaseInfo();
private:
	float m_fMoveDistance;
	float m_fElapsedLifeTime;

	SGActor* m_pSpawner;
	SGProjectileInfo* m_pBaseInfo;
};