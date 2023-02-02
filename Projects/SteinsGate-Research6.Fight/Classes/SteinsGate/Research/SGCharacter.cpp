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
#include <SteinsGate/Research/SGActionDefine.h>

#include <SteinsGate/Common/Engine/SGRectEx.h>



USING_NS_JC;
USING_NS_CC;

SGCharacter::SGCharacter(int code, const SGCharacterInfo& info)
	: SGPhysicsActor(ActorType::Character, code)
	, m_CharacterInfo(info)
	, m_bOwner(false)
{
}

SGCharacter* SGCharacter::create(int code, const SGCharacterInfo& info) {
	SGCharacter* pCharacter = new SGCharacter(code, info);

	if (pCharacter && pCharacter->init()) {
		SGCharacterBaseInfo* pBaseInfo = SGDataManager::getInstance()->getCharacterBaseInfo(code);
		pCharacter->m_pBaseInfo = pBaseInfo;
		pCharacter->initThicknessBox(pBaseInfo->ThicknessBox);
		pCharacter->initActorSprite();
		pCharacter->initHitRecorder(32, 64);
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

void SGCharacter::initListener(SGActorListener* listener) {
	// 캐릭터가 다양해지면 필요할듯
}


void SGCharacter::hit(const SGHitInfo& hitInfo) {
	SGPhysicsActor::hit(hitInfo);

	if (!m_bOwner)
		return;

	SGPlayer* pPlayer = SGPlayer::getInstance();

	if (hitInfo.AttackDataInfo->IsFallDownAttack) {
		pPlayer->runActionForce(GUNNER_ACTION_FALL_DOWN);
		return;
	}

	pPlayer->runActionForce(GUNNER_ACTION_HIT);
}

void SGCharacter::update(float dt) {
	SGPhysicsActor::update(dt);
}

void SGCharacter::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {


	if (!m_bOwner)
		return;


	SGPlayer::getInstance()->onFrameBegin(animation, texture);
}

void SGCharacter::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {

	if (!m_bOwner)
		return;

	SGPlayer::getInstance()->onFrameEnd(animation, texture);
}

void SGCharacter::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {

	if (!m_bOwner)
		return;

	SGPlayer::getInstance()->onAnimationBegin(animation, texture);
}

void SGCharacter::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {

	if (!m_bOwner)
		return;

	SGPlayer::getInstance()->onAnimationEnd(animation, texture);
}

void SGCharacter::setOwner(bool owner) {
	m_bOwner = owner;
}

SGCharacterBaseInfo* SGCharacter::getBaseInfo() {
	return m_pBaseInfo;
}

