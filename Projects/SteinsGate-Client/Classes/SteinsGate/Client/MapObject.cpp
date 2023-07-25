/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 4:37:14 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "MapObject.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/ActorSprite.h>
#include <SteinsGate/Client/AnimationDefine.h>

USING_NS_CC;
USING_NS_JC;

MapObject::MapObject(MapObjectInfo* baseInfo)
	: Actor(ActorType::MapObject, baseInfo->Code)
	, m_pBaseInfo(baseInfo)
{
}

MapObject* MapObject::create(MapObjectInfo* baseInfo) {
	MapObject* pMapObject = dbg_new MapObject(baseInfo);

	pMapObject->initThicknessBox(baseInfo->ThicknessBox);
	pMapObject->initActorSprite();
	pMapObject->initVariables();
	pMapObject->autorelease();

	return pMapObject;
}

// 옵스터클은 파츠, 애니메이션 다 1개씩임 (팔 다리 달린 장애물은 없을 듯?)
void MapObject::initActorSprite() {
	m_pActorSprite = ActorSprite::create(this, m_pBaseInfo->SpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	m_pActorSprite->runAnimation(1);
	this->addChild(m_pActorSprite);

	switch (m_pBaseInfo->Type) {
	case MapObjectType::Obstacle:	initActorSpriteObstacle();	break;
	case MapObjectType::Gate:		initActorSpriteGate();		break;
	}
}

void MapObject::initActorSpriteObstacle() {
	//MapObjectObstacleInfo* pInfo = dynamic_cast<MapObjectObstacleInfo*>(m_pBaseInfo);
	//ActorSpriteDataPtr spActorSpriteData = MakeShared<ActorSpriteData>(1, pInfo->SpriteData.Animations.Size());

	//// 장애물은 애니메이션이 1개
	//spActorSpriteData->Parts.PushBack({ 1, pInfo->SgaIndex, pInfo->ImgIndex });

	//for (int i = 0; i < pInfo->AnimationList.Size(); ++i) {
	//	spActorSpriteData->Animations.PushBack(&pInfo->AnimationList[i]);
	//}

	//
}

void MapObject::initActorSpriteGate() {
	//MapObjectGateInfo* pInfo = dynamic_cast<MapObjectGateInfo*>(m_pBaseInfo);
	//ActorSpriteDataPtr spActorSpriteData = MakeShared<ActorSpriteData>(MaxMapObjectGatePartsCount_v, 1); // 게이트는 싱글 프레임 싱글 애니메이션을 사용

	//for (int i = 0; i < MaxMapObjectGatePartsCount_v; ++i) {
	//	ActorPartSpriteData& part = pInfo->Parts[i];
	//	spActorSpriteData->Parts.PushBack({ part.ZOrder, part.SgaIndex, part.ImgIndex });
	//}

	//spActorSpriteData->Animations.PushBack(&pInfo->Animation);

	//m_pActorSprite = ActorSprite::create(this, spActorSpriteData);
	//m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	//this->addChild(m_pActorSprite);
}

bool MapObject::initVariables() {
	return Actor::initVariables();

}

void MapObject::update(float dt) {
	Actor::update(dt);
}

void MapObject::onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {

}

void MapObject::onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {

}

void MapObject::onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) {

}

void MapObject::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) {

}

MapObjectInfo* MapObject::getBaseInfo() {
	return m_pBaseInfo;
}
