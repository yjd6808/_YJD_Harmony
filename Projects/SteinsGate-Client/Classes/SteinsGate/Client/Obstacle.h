/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 4:37:02 PM
 * =====================
 *
 */


#pragma once





#include <SteinsGate/Client/Actor.h>
#include <SteinsGate/Client/ObstacleInfo.h>

class Obstacle : public Actor
{
public:
	Obstacle(ObstacleInfo* baseInfo);
	static Obstacle* create(ObstacleInfo* baseInfo);
	void initActorSprite() override;
	void initListener(ActorListener* listener) override;
	bool initVariables() override;

	void update(float dt) override;
	void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) override;

	bool hitable();
	bool collidable();
	bool z_orderable();

	ObstacleInfo* getBaseInfo();
private:
	ObstacleInfo* m_pBaseInfo;
};


