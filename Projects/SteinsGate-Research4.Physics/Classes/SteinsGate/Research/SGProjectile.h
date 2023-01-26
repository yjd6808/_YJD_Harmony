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
	void initActorSprite() override;
	void initThicknessBox(const SGThicknessBox& thicknessBox) override;

	void update(float dt) override;
	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;

	void addHitActor(SGActor* actor);
	void removeHitActor(SGActor* actor);

	float isLifeTimeOver();
	float isDistanceOver();
	SGActor* getSpawner();
	SGProjectileInfo* getBaseInfo();
private:
	float m_fMoveDistance;
	float m_fElapsedLifeTime;

	SGActor* m_pSpwawner;
	SGProjectileInfo* m_pBaseInfo;
	SGCharacterInfo m_CharacterInfo;
	SGVector<SGActor*> m_vHitList;
};