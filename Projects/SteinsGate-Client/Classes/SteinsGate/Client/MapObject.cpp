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
#include <SteinsGate/Client/Define_Animation.h>

USING_NS_CC;
USING_NS_JC;

MapObject::MapObject(MapObjectInfo* baseInfo) : m_pBaseInfo(baseInfo) {}

MapObject* MapObject::create(MapObjectInfo* baseInfo) {
	MapObject* pMapObject = dbg_new MapObject(baseInfo);
	pMapObject->initialize();
	pMapObject->autorelease();
	return pMapObject;
}

void MapObject::initialize() {
	initThicknessBox(m_pBaseInfo->ThicknessBox);
	initActorSprite();
	initVariables();
	initListeners();
	initComponents();
}

// 옵스터클은 파츠, 애니메이션 다 1개씩임 (팔 다리 달린 장애물은 없을 듯?)
void MapObject::initActorSprite() {

	if (!m_pActorSprite) {
		m_pActorSprite = ActorSprite::create(this, m_pBaseInfo->SpriteData);
		m_pActorSprite->setAnchorPoint(Vec2::ZERO);
		m_pActorSprite->runAnimation(1);
		this->addChild(m_pActorSprite);	
	}

	switch (m_pBaseInfo->Type) {
	case MapObjectType::Obstacle:	initActorSpriteObstacle();	break;
	case MapObjectType::Gate:		initActorSpriteGate();		break;
	}
}

void MapObject::initActorSpriteObstacle() {}
void MapObject::initActorSpriteGate() {}

bool MapObject::initVariables() {
	return Actor::initVariables();
}

void MapObject::initListeners() {
}

void MapObject::initComponents() {
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
