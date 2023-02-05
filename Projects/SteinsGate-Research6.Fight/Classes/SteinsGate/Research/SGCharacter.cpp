/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:07 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGCharacter.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGActorSprite.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGHostPlayer.h>
#include <SteinsGate/Research/SGActionDefine.h>
#include <SteinsGate/Research/SGActorBox.h>

#include <SteinsGate/Common/Engine/SGRectEx.h>
#include <SteinsGate/Common/Engine/SGRectEx.h>


USING_NS_JC;
USING_NS_CC;

SGCharacter::SGCharacter(int code, const SGCharacterInfo& info)
	: SGPhysicsActor(ActorType::Character, code)
	, m_CharInfo(info)
	, m_bOwner(false)
{
}

SGCharacter::~SGCharacter() {
	Log("캐릭터 소멸\n");
}

SGCharacter* SGCharacter::create(int code, const SGCharacterInfo& info) {
	SGCharacter* pCharacter = new SGCharacter(code, info);

	if (pCharacter && pCharacter->init()) {
		SGCharBaseInfo* pBaseInfo = SGDataManager::get()->getCharBaseInfo(code);
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
	SGDataManager* pDataManager = SGDataManager::get();
	SGImagePackManager* pImgPackManager = SGImagePackManager::get();
	AnimationList& animationList = pDataManager->getCharAnimationInfoList(m_iCode);
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>(15, animationList.Size());

	for (int i = 0; i < VisualType::Max; ++i) {
		if (m_CharInfo.VisualInfo.ImgIndex[i] != InvalidValue_v &&
			m_CharInfo.VisualInfo.NpkIndex[i] != InvalidValue_v) {
			spActorSpriteData->Parts.PushBack({
				m_pBaseInfo->DefaultVisualZOrder[i],
				m_CharInfo.VisualInfo.NpkIndex[i],
				m_CharInfo.VisualInfo.ImgIndex[i]
			});
		}
	}
	spActorSpriteData->Animations = animationList;

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

	SGHostPlayer* pPlayer = SGHostPlayer::get();

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


	SGHostPlayer::get()->onFrameBegin(animation, texture);
}

void SGCharacter::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {

	if (!m_bOwner)
		return;

	SGHostPlayer::get()->onFrameEnd(animation, texture);
}

void SGCharacter::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {

	if (!m_bOwner)
		return;

	SGHostPlayer::get()->onAnimationBegin(animation, texture);
}

void SGCharacter::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {

	if (!m_bOwner)
		return;

	SGHostPlayer::get()->onAnimationEnd(animation, texture);
}

void SGCharacter::setOwner(bool owner) {
	m_bOwner = owner;
}

void SGCharacter::cleanUpImmediate() {
	m_bCleanUp = true;
	m_pMapLayer = nullptr;
	SGActorBox::get()->cleanUpCharacter(this);
}

SGCharBaseInfo* SGCharacter::getBaseInfo() {
	return m_pBaseInfo;
}

