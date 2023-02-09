/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:45 AM
 * =====================
 *
 */

#include "SGMonster.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGActorSprite.h>
#include <SteinsGate/Research/SGAnimationDefine.h>

USING_NS_CC;
USING_NS_JC;

SGMonster::SGMonster(SGMonsterInfo* baseInfo, SGMapLayer* mapLayer)
	: SGPhysicsActor(ActorType::Monster, baseInfo->Code, mapLayer)
	, m_pBaseInfo(baseInfo)
{
}

SGMonster* SGMonster::create(SGMonsterInfo* baseInfo, SGMapLayer* mapLayer) {
	SGMonster* pMonster = new SGMonster(baseInfo, mapLayer);

	if (pMonster && pMonster->init()) {
		pMonster->initThicknessBox(baseInfo->ThicknessBox);
		pMonster->initActorSprite();
		pMonster->autorelease();
		return pMonster;
	}

	return pMonster;
}

void SGMonster::initActorSprite() {
	SGDataManager* pDataManager = SGDataManager::getInstance();
	SGImagePackManager* pImgPackManager = SGImagePackManager::getInstance();
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>(m_pBaseInfo->PartsCount, m_pBaseInfo->AnimationList.Size());

	for (int i = 0; i < m_pBaseInfo->PartsCount; ++i) {
		SGMonsterPartInfo& part = m_pBaseInfo->Parts[i];
		spActorSpriteData->Parts.PushBack({ part.ZOrder, part.PackIndex, part.ImgIndex });
	}

	for (int i = 0; i < m_pBaseInfo->AnimationList.Size(); ++i) {
		spActorSpriteData->Animations.PushBack(&m_pBaseInfo->AnimationList[i]);
	}

	m_pActorSprite = SGActorSprite::create(this, spActorSpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	m_pActorSprite->runAnimation(MONSTER_ANIMATION_IDLE);
	this->addChild(m_pActorSprite);
}


void SGMonster::update(float dt) {
	SGPhysicsActor::update(dt);

}

void SGMonster::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
}

void SGMonster::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
}

void SGMonster::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	
}

void SGMonster::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	
}

SGMonsterInfo* SGMonster::getBaseInfo() {
	return m_pBaseInfo;
}