/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 4:37:02 PM
 * =====================
 *
 */


#pragma once





#include <SteinsGate/Client/Actor.h>
#include <SteinsGate/Client/MapObjectInfo.h>

class MapObject : public Actor
{
public:
	MapObject(MapObjectInfo* baseInfo);
	static MapObject* create(MapObjectInfo* baseInfo);
	void initActorSprite() override;
	void initActorSpriteObstacle();
	void initActorSpriteGate();
	bool initVariables() override;

	void update(float dt) override;
	void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) override;

	MapObjectType_t getObjectType() const { return m_pBaseInfo->Type; }
	int getCode() override { return m_pBaseInfo->Code; }

	MapObjectInfo* getBaseInfo();
private:
	MapObjectInfo* m_pBaseInfo;
};


