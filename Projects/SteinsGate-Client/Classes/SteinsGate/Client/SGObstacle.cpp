﻿/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 4:37:14 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "SGObstacle.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGActorSprite.h>
#include <SteinsGate/Client/SGAnimationDefine.h>

USING_NS_CC;
USING_NS_JC;

SGObstacle::SGObstacle(SGObstacleInfo* baseInfo)
	: SGActor(ActorType::Obstacle, baseInfo->Code)
	, m_pBaseInfo(baseInfo)
{
}

SGObstacle* SGObstacle::create(SGObstacleInfo* baseInfo) {
	SGObstacle* pObstable = dbg_new SGObstacle(baseInfo);

	pObstable->initThicknessBox(baseInfo->ThicknessBox);
	pObstable->initActorSprite();
	pObstable->initVariables();
	pObstable->autorelease();

	return pObstable;
}

// 옵스터클은 파츠, 애니메이션 다 1개씩임 (팔 다리 달린 장애물은 없을 듯?)
void SGObstacle::initActorSprite() {
	SGDataManager* pDataManager = SGDataManager::get();
	SGImagePackManager* pImgPackManager = SGImagePackManager::get();
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>(1, m_pBaseInfo->AnimationList.Size());	

	spActorSpriteData->Parts.PushBack({ 1, m_pBaseInfo->SgaIndex, m_pBaseInfo->ImgIndex });

	for (int i = 0; i < m_pBaseInfo->AnimationList.Size(); ++i) {
		spActorSpriteData->Animations.PushBack(&m_pBaseInfo->AnimationList[i]);
	}

	m_pActorSprite = SGActorSprite::create(this, spActorSpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	m_pActorSprite->runAnimation(1);
	this->addChild(m_pActorSprite);
}

void SGObstacle::initListener(SGActorListener* listener) {
	DebugAssertMsg(false, "아직 구현안됨");
}

bool SGObstacle::initVariables() {
	return SGActor::initVariables();

}

void SGObstacle::update(float dt) {
	SGActor::update(dt);
}

void SGObstacle::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {

}

void SGObstacle::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {

}

void SGObstacle::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {

}

void SGObstacle::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {

}

bool SGObstacle::hitable() {
	return m_pBaseInfo->Hitable;
}

bool SGObstacle::collidable() {
	return m_pBaseInfo->Colliadalble;
}

bool SGObstacle::z_orderable() {
	return m_pBaseInfo->ZOrederable;
}


SGObstacleInfo* SGObstacle::getBaseInfo() {
	return m_pBaseInfo;
}