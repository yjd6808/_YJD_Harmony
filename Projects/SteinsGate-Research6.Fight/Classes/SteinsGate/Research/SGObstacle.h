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
	SGObstacle(SGObstacleInfo* baseInfo);
	static SGObstacle* create(SGObstacleInfo* baseInfo);
	void initActorSprite() override;
	void initListener(SGActorListener* listener) override;
	bool initVariables() override;

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


