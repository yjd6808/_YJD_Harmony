/*
 * 작성자: 윤정도
 * 생성일: 1/25/2023 4:37:14 PM
 * =====================
 *
 */




#include "SGObstacle.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGActorSprite.h>
#include <SteinsGate/Research/SGAnimationDefine.h>

USING_NS_CC;
USING_NS_JC;

SGObstacle::SGObstacle(SGObstacleInfo* baseInfo)
	: SGActor(ActorType::Obstacle, baseInfo->Code)
	, m_pBaseInfo(baseInfo)
{
}

SGObstacle* SGObstacle::create(SGObstacleInfo* baseInfo) {
	SGObstacle* pObstable = new SGObstacle(baseInfo);

	if (pObstable && pObstable->init()) {
		pObstable->initThicknessBox(baseInfo->ThicknessBox);
		pObstable->initActorSprite();
		pObstable->autorelease();
		return pObstable;
	}

	return pObstable;
}

// 옵스터클은 파츠, 애니메이션 다 1개씩임 (팔 다리 달린 장애물은 없을 듯?)
void SGObstacle::initActorSprite() {
	SGDataManager* pDataManager = SGDataManager::getInstance();
	SGImagePackManager* pImgPackManager = SGImagePackManager::getInstance();
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>(1, m_pBaseInfo->AnimationList.Size());	

	spActorSpriteData->Parts.PushBack({ 1, m_pBaseInfo->NpkIndex, m_pBaseInfo->ImgIndex });

	for (int i = 0; i < m_pBaseInfo->AnimationList.Size(); ++i) {
		spActorSpriteData->Animations.PushBack(&m_pBaseInfo->AnimationList[i]);
	}

	m_pActorSprite = SGActorSprite::create(this, spActorSpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	m_pActorSprite->runAnimation(1);
	this->addChild(m_pActorSprite);
}

void SGObstacle::initListener(SGActorListener* listener) {
	DebugAssertMessage(false, "아직 구현안됨");
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
