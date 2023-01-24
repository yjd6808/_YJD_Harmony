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

SGMonster::SGMonster(SGMonsterInfo* baseInfo)
	: SGActor(ActorType::Monster, baseInfo->Code)
	, m_pBaseInfo(baseInfo)
{
}

SGMonster* SGMonster::create(SGMonsterInfo* baseInfo) {
	SGMonster* pMonster = new SGMonster(baseInfo);

	if (pMonster && pMonster->init()) {
		pMonster->initThicknessBox({ baseInfo->ThicknessBoxWidth, baseInfo->ThicknessBoxHeight, baseInfo->ThicknessBoxRelativeY });
		pMonster->initActorSprite();
		pMonster->autorelease();
		return pMonster;
	}

	return pMonster;
}

void SGMonster::initActorSprite() {
	SGDataManager* pConfig = SGDataManager::getInstance();
	SGImagePackManager* pImgPackManager = SGImagePackManager::getInstance();
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>();

	for (int i = 0; i < m_pBaseInfo->PartsCount; ++i) {
		SGMonsterPartInfo& part = m_pBaseInfo->Parts[i];
		spActorSpriteData->Parts.PushBack({ part.ZOrder, part.PackIndex, part.ImgIndex });
	}

	for (int i = 0; i < m_pBaseInfo->Animations.Size(); ++i) {
		spActorSpriteData->Animations.PushBack(&m_pBaseInfo->Animations[i]);
	}

	m_pActorSprite = SGActorSprite::create(this, spActorSpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	m_pActorSprite->runAnimation(MONSTER_ANIMATION_WALK);
	this->addChild(m_pActorSprite);
}

void SGMonster::update(float dt) {
	SGActor::update(dt);
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