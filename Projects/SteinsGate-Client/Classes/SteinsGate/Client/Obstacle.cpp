/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 4:37:14 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "Obstacle.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/ActorSprite.h>
#include <SteinsGate/Client/AnimationDefine.h>

USING_NS_CC;
USING_NS_JC;

Obstacle::Obstacle(ObstacleInfo* baseInfo)
	: Actor(ActorType::Obstacle, baseInfo->Code)
	, m_pBaseInfo(baseInfo)
{
}

Obstacle* Obstacle::create(ObstacleInfo* baseInfo) {
	Obstacle* pObstable = dbg_new Obstacle(baseInfo);

	pObstable->initThicknessBox(baseInfo->ThicknessBox);
	pObstable->initActorSprite();
	pObstable->initVariables();
	pObstable->autorelease();

	return pObstable;
}

// 옵스터클은 파츠, 애니메이션 다 1개씩임 (팔 다리 달린 장애물은 없을 듯?)
void Obstacle::initActorSprite() {
	DataManager* pDataManager = DataManager::get();
	ImagePackManager* pImgPackManager = ImagePackManager::get();
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>(1, m_pBaseInfo->AnimationList.Size());	

	spActorSpriteData->Parts.PushBack({ 1, m_pBaseInfo->SgaIndex, m_pBaseInfo->ImgIndex });

	for (int i = 0; i < m_pBaseInfo->AnimationList.Size(); ++i) {
		spActorSpriteData->Animations.PushBack(&m_pBaseInfo->AnimationList[i]);
	}

	m_pActorSprite = ActorSprite::create(this, spActorSpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	m_pActorSprite->runAnimation(1);
	this->addChild(m_pActorSprite);
}

void Obstacle::initListener(ActorListener* listener) {
	DebugAssertMsg(false, "아직 구현안됨");
}

bool Obstacle::initVariables() {
	return Actor::initVariables();

}

void Obstacle::update(float dt) {
	Actor::update(dt);
}

void Obstacle::onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {

}

void Obstacle::onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {

}

void Obstacle::onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) {

}

void Obstacle::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) {

}

bool Obstacle::hitable() {
	return m_pBaseInfo->Hitable;
}

bool Obstacle::collidable() {
	return m_pBaseInfo->Colliadalble;
}

bool Obstacle::z_orderable() {
	return m_pBaseInfo->ZOrederable;
}


ObstacleInfo* Obstacle::getBaseInfo() {
	return m_pBaseInfo;
}
