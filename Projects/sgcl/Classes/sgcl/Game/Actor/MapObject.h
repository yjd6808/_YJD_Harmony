/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 4:37:02 PM
 * =====================
 *
 */

#pragma once

#include "sgcl/Game/Actor/Actor.h"
#include "sg/Struct/SteinsGate_MapObject.h"

class MapObject : public Actor
{
public:
	MapObject(MapObjectInfo* _pBaseInfo);
	static MapObject* Create(MapObjectInfo* _pBaseInfo);

	void Initialize() override;
	void InitActorSprite() override;
	void InitActorSpriteObstacle();
	void InitActorSpriteGate();
	bool InitVariables() override;
	void InitListeners() override;
	void InitComponents() override;

	void update(float _dt) override;
	void OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;

	MapObjectType_t GetObjectType() const;
	ActorType_t GetType() const override;

	int GetCode() override;

	MapObjectInfo* GetBaseInfo();

private:
	MapObjectInfo* pBaseInfo_;
};
