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
	MapObject(MapObjectInfo* _pBaseInfo);
	static MapObject* create(MapObjectInfo* _pBaseInfo);

	void initialize() override;
	void initActorSprite() override;
	void initActorSpriteObstacle();
	void initActorSpriteGate();
	bool initVariables() override;
	void initListeners() override;
	void initComponents() override;

	void update(float _dt) override;
	void onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;

	MapObjectType_t getObjectType() const;
	ActorType_t getType() const override;

	int getCode() override;

	MapObjectInfo* getBaseInfo();

private:
	MapObjectInfo* baseInfo_;
};
