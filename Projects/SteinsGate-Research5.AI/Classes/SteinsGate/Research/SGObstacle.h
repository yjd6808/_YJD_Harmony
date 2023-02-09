/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 4:37:02 PM
 * =====================
 *
 */


#pragma once





#include <SteinsGate/Research/SGActor.h>
#include <SteinsGate/Research/SGObstacleInfo.h>

class SGObstacle : public SGActor
{
public:
	SGObstacle(SGObstacleInfo* baseInfo, SGMapLayer* mapLayer);
	static SGObstacle* create(SGObstacleInfo* baseInfo, SGMapLayer* mapLayer);
	void initActorSprite() override;

	void update(float dt) override;
	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;

	bool hitable();
	bool collidable();
	bool z_orderable();

	SGObstacleInfo* getBaseInfo();
private:
	SGObstacleInfo* m_pBaseInfo;
};


