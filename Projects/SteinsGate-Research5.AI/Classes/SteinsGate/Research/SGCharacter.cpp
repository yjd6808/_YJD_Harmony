/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:07 AM
 * =====================
 *
 */


#include "SGCharacter.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGActorSprite.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGPlayer.h>

#include <SteinsGate/Common/Engine/SGRectEx.h>

USING_NS_JC;
USING_NS_CC;

SGCharacter::SGCharacter(int code, const SGCharacterInfo& info, SGMapLayer* mapLayer)
	: SGPhysicsActor(ActorType::Character, code, mapLayer)
	, m_CharacterInfo(info)
{
}

SGCharacter* SGCharacter::create(int code, const SGCharacterInfo& info, SGMapLayer* mapLayer) {
	SGCharacter* pCharacter = new SGCharacter(code, info, mapLayer);

	if (pCharacter && pCharacter->init()) {
		SGCharacterBaseInfo* pBaseInfo = SGDataManager::getInstance()->getCharacterBaseInfo(code);
		pCharacter->m_pBaseInfo = pBaseInfo;
		pCharacter->initThicknessBox(pBaseInfo->ThicknessBox);
		pCharacter->initActorSprite();
		pCharacter->autorelease();
		return pCharacter;
	}

	return pCharacter;
}

void SGCharacter::initActorSprite() {
	SGDataManager* pDataManager = SGDataManager::getInstance();
	SGImagePackManager* pImgPackManager = SGImagePackManager::getInstance();
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>(15, m_CharacterInfo.ValidAction.Size());

	for (int i = 0; i < VisualType::Max; ++i) {
		if (m_CharacterInfo.VisualInfo.ImgIndex[i] != InvalidValue_v &&
			m_CharacterInfo.VisualInfo.NpkIndex[i] != InvalidValue_v) {
			spActorSpriteData->Parts.PushBack({
				m_pBaseInfo->DefaultVisualZOrder[i],
				m_CharacterInfo.VisualInfo.NpkIndex[i],
				m_CharacterInfo.VisualInfo.ImgIndex[i]
			});
		}
	}

	SGVector<int>& vActions = m_CharacterInfo.ValidAction;

	for (int i = 0; i < vActions.Size(); ++i) {
		SGActionInfo* pActionInfo = pDataManager->getActionInfo(vActions[i]);
		for (int j = 0; j < pActionInfo->AnimationList.Size(); ++j) {
			spActorSpriteData->Animations.PushBack(&pActionInfo->AnimationList[j]);
		}
	}

	m_pActorSprite = SGActorSprite::create(this, spActorSpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pActorSprite);
}

void SGCharacter::update(float dt) {
	SGPhysicsActor::update(dt);
}

extern SGPlayer* MainPlayer_v;


void SGCharacter::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	MainPlayer_v->onFrameBegin(animation, texture);
}

void SGCharacter::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	MainPlayer_v->onFrameEnd(animation, texture);
}

void SGCharacter::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	MainPlayer_v->onAnimationBegin(animation, texture);
}

void SGCharacter::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	MainPlayer_v->onAnimationEnd(animation, texture);
}

SGCharacterBaseInfo* SGCharacter::getBaseInfo() {
	return m_pBaseInfo;
}

