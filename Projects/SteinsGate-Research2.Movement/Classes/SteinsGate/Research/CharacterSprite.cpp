/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 4:01:23 AM
 * =====================
 *
 */


#include "CharacterSprite.h"

#include <SteinsGate/Research/Config.h>
#include <SteinsGate/Research/PackLoader.h>

#include <SteinsGate/Research/CharacterPartSpriteFrame.h>


using namespace cocos2d;

CharacterSprite* CharacterSprite::create(CharacterType_t chracterType)
{
	auto c = new (std::nothrow) CharacterSprite(chracterType);

	if (c && c->init()) {
		c->autorelease();
		return c;
	}

	CC_SAFE_DELETE(c);
	return nullptr;
}

bool CharacterSprite::init()
{
	if (!Sprite::init())
		return false;


	m_pCanvas = Node::create();
	m_pCanvas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	this->addChild(m_pCanvas, 0);

	for (int ePartType = 0; ePartType < CharacterPartType::Max; ++ePartType) {
		CharacterPartSprite* pPart = CharacterPartSprite::create(this, (CharacterPartType_t)ePartType, CharacterFrameCount_v[m_eCharacterType]);
		pPart->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		m_pCanvas->addChild(pPart, CharacterPartZOrders_v[ePartType]);
		m_vParts.PushBack(pPart);

		
		for (int j = 0; j < MotionState::Max; ++j) {
			const int iBegin = CharacterMotionDefailIndexPairs_v[j].item1;
			const int iEnd = CharacterMotionDefailIndexPairs_v[j].item2;

			DebugAssert(iBegin <= iEnd);

			Animation* pAnimation = Animation::create();
			pAnimation->setDelayPerUnit(CharacterSpriteFrameDelay_v);
			
			for (int iFrameIndex = iBegin, iFrameIndexInAnimation = 0; iFrameIndex <= iEnd; iFrameIndex++, iFrameIndexInAnimation++) {
				pPart->getFrame(iFrameIndex)->m_iFrameIndexInAnimation = iFrameIndexInAnimation;
				pAnimation->addSpriteFrame(pPart->getFrame(iFrameIndex));
			}

			auto pAnimate = CharacterPartAnimate::create(pAnimation, pPart);
			pAnimate->retain();
			pPart->addAnimation(j, pAnimate);
		}

		int imgIndex = CharacterPartDefaultImg::Get(m_eCharacterType, (CharacterPartType_t)ePartType);
		changePart((CharacterPartType_t)ePartType, imgIndex);
	}

	setIdle();

	return true;
}

void CharacterSprite::update(float delta)
{

}

void CharacterSprite::changePart(CharacterPartType_t partType, int imgIndex)
{
	const int iMaxFrameCount = CharacterFrameCount_v[m_eCharacterType];

	for (int i = 0; i < iMaxFrameCount; ++i) {
		FrameTexturePtr pTexture = PackLoader::GetInstance()->CreateFrameTexture(partType, imgIndex, i);

		if (!pTexture.Exist())
			continue;

		auto pPart = m_vParts[partType];
		auto pBeforeFrame = pPart->getFrame(i);		// 텍스쳐 변경전 프레임
		int iTargetFrameIndex = pTexture->getTargetFrameIndex();

		if (pTexture->isLink()) {
			DebugAssertMessage(iTargetFrameIndex >= 0, "타겟 인덱스가 음수입니다.");
			DebugAssertMessage(iTargetFrameIndex < i, "이전 텍스쳐가 생성되기전에 링크를 시도할려고 합니다.");

			CharacterPartSpriteFrame* pPartSpriteFrame = pPart->getFrame(iTargetFrameIndex);
			FrameTexture* pTargetTexture = pPartSpriteFrame->getFrameTexture();
			pBeforeFrame->initWithFrameTexture(pTargetTexture);
			continue;
		}

		pBeforeFrame->initWithFrameTexture(pTexture);
	}
}

void CharacterSprite::setForwardDirection() {
	m_pCanvas->setRotationSkewY(0);
	m_eDirection = SpriteDirection::Right;
}

void CharacterSprite::setBackwardDirection() {
	m_pCanvas->setRotationSkewY(180.0f);
	m_eDirection = SpriteDirection::Left;
}

void CharacterSprite::setCanvasPosition(const cocos2d::Vec2& pos) {
	m_pCanvas->setPosition(pos);
}

cocos2d::Vec2 CharacterSprite::getCanvasPosition() {
	return m_pCanvas->getPosition();
}

void CharacterSprite::setIdle() {
	// 코코스 백터를 인자로 받아버려서, 내가만든벡터로 전달할 수가 없네;;
	cocos2d::Vector<FiniteTimeAction*> sequences{ CharacterPartType::Max };

	for (int ePartType = 0; ePartType < CharacterPartType::Max; ++ePartType) {
		sequences.pushBack(m_vParts[ePartType]->getAnimation(MotionState::IdleBreath));
		sequences.pushBack(m_vParts[ePartType]->getAnimation(MotionState::IdleBreath));
		sequences.pushBack(m_vParts[ePartType]->getAnimation(MotionState::IdleBreath));
		sequences.pushBack(m_vParts[ePartType]->getAnimation(MotionState::IdleBreath));
		sequences.pushBack(m_vParts[ePartType]->getAnimation(MotionState::IdleBreath));
		sequences.pushBack(m_vParts[ePartType]->getAnimation(MotionState::IdleBreath));
		sequences.pushBack(m_vParts[ePartType]->getAnimation(MotionState::IdleReload));
		sequences.pushBack(m_vParts[ePartType]->getAnimation(MotionState::IdleGunRolling));

		m_vParts[ePartType]->runSequenceRepeat(Sequence::create(sequences));
		sequences.clear();
	}

	m_eMotionState = MotionState::IdleBegin;
}

void CharacterSprite::setWalk()
{
	for (int ePartType = 0; ePartType < CharacterPartType::Max; ++ePartType) {
		m_vParts[ePartType]->runSequenceRepeat(Sequence::create(m_vParts[ePartType]->getAnimation(MotionState::Walk), nullptr));
	}

	m_eMotionState = MotionState::Walk;
}

void CharacterSprite::setRunning() {
	for (int ePartType = 0; ePartType < CharacterPartType::Max; ++ePartType) {
		m_vParts[ePartType]->runSequenceRepeat(Sequence::create(m_vParts[ePartType]->getAnimation(MotionState::Run), nullptr));
	}

	m_eMotionState = MotionState::Run;
}

void CharacterSprite::setSlide(SpriteDirection_t direciton, CallFunc* slideBegin, CallFunc* slideEnd) {
	stopAllActions();

	// https://home.deds.nl/~jessez/jessezoutewelle.nl/caurina/transitions/TransitionCheatSheets_2/Tweener%20-%20Transition%20cheat%20sheet%20-%20Complete.pdf
	EaseCircleActionOut* pEaseOutByDirection = direciton == SpriteDirection::Right ?
		EaseCircleActionOut::create(MoveBy::create(SlidingDuration_v, Vec2{  SlidingMoveByDistance_v, 0.0 })) :
		EaseCircleActionOut::create(MoveBy::create(SlidingDuration_v, Vec2{ -SlidingMoveByDistance_v, 0.0 }));

	runAction(Sequence::create(
		DelayTime::create(SlidingBeforeDelay_v),
		slideBegin,
		pEaseOutByDirection,
		slideEnd,
		nullptr
	));

	for (int ePartType = 0; ePartType < CharacterPartType::Max; ++ePartType) {
		m_vParts[ePartType]->runSequenceRepeat(Sequence::create(m_vParts[ePartType]->getAnimation(MotionState::Sliding), nullptr));
	}
}

bool CharacterSprite::isIdleMotion() {
	return m_eMotionState >= MotionState::IdleBegin && m_eMotionState <= MotionState::IdleEnd;
}


bool CharacterSprite::isWalkingMotion() {
	return m_eMotionState == MotionState::Walk;
}

cocos2d::DrawNode* CharacterSprite::getSkinBoundingBox() {
	return m_vParts[CharacterPartType::Skin]->getDrawNode();
}


bool CharacterSprite::isRunningMotion() {
	return m_eMotionState == MotionState::Run;
}

bool CharacterSprite::isShootingMotion() {
	return m_eMotionState >= MotionState::ShotBegin && m_eMotionState <= MotionState::ShotEnd;
}

bool CharacterSprite::isJumpMotion() {
	return m_eMotionState >= MotionState::JumpBegin && m_eMotionState <= MotionState::JumpEnd;
}

// CharacterPartType::Skin
	// CharacterPartType::Shoes
	// CharacterPartType::Pants
	// CharacterPartType::Neck
	// CharacterPartType::Hair
	// CharacterPartType::Face
	// CharacterPartType::Coat
	// CharacterPartType::Cap
	// CharacterPartType::Belt
	// CharacterPartType::Weapon