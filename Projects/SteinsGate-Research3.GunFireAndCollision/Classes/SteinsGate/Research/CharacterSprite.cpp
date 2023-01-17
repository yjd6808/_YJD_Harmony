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

	
	// m_pCanvas = Node::create();
	m_pCanvas = Sprite::create("rect.png");
	m_pCanvas->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_pCanvas->setCascadeOpacityEnabled(false);
	m_pCanvas->setOpacity(0);
	m_pCanvas->setPosition(-250.0f, -250.0f);
	m_pCanvas->setContentSize({ 500.0f, 500.0f });
	this->addChild(m_pCanvas, 0);


	for (int ePartType = 0; ePartType < CharacterPartType::Max; ++ePartType) {
		CharacterPartSprite* pPart = CharacterPartSprite::create(this, (CharacterPartType_t)ePartType, CharacterFrameCount_v[m_eCharacterType]);
		pPart->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		m_pCanvas->addChild(pPart, CharacterPartZOrders_v[ePartType]);
		m_vParts.PushBack(pPart);

		
		for (int j = 0; j < MotionState::Max; ++j) {
			const int iBegin = MotionStatePairs_v[j].item1;
			const int iEnd = MotionStatePairs_v[j].item2;
			const int iIndex = MotionDetail_TypeMap[iBegin];

			DebugAssert(iBegin <= iEnd);

			Animation* pAnimation = Animation::create();



			if (iIndex == MotionState::ShotRightShot ||
				iIndex == MotionState::ShotRightDownShot ||
				iIndex == MotionState::ShotLeftShot ||
				iIndex == MotionState::ShotLeftDownShot) {
				pAnimation->setDelayPerUnit(FPS4_v);
			} else if (iIndex == MotionState::IdleBreath) {
				pAnimation->setDelayPerUnit(FPS9_v);
			} else if (iIndex == MotionState::IdleReload) {
				pAnimation->setDelayPerUnit(FPS9_v);

			} else {
				pAnimation->setDelayPerUnit(CharacterSpriteFrameDelay_v);
			}
			Log("%.1f\n", pAnimation->getDelayPerUnit());
			
			for (int iFrameIndex = iBegin, iFrameIndexInAnimation = 0; iFrameIndex <= iEnd; iFrameIndex++, iFrameIndexInAnimation++) {
				pPart->getFrame(iFrameIndex)->m_iFrameIndexInAnimation = iFrameIndexInAnimation;
				pAnimation->addSpriteFrame(pPart->getFrame(iFrameIndex));
			}

			auto pAnimate = CharacterPartAnimate::create(pAnimation, pPart, iIndex);
			pAnimate->retain();
			pPart->addAnimation(j, pAnimate);
		}

		int imgIndex = CharacterPartDefaultImg::Get(m_eCharacterType, (CharacterPartType_t)ePartType);
		changePart((CharacterPartType_t)ePartType, imgIndex);
	}

	setIdle();

	return true;
}

void CharacterSprite::update(float delta) {

}

void CharacterSprite::changePart(CharacterPartType_t partType, int imgIndex) {
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

cocos2d::Vec2 CharacterSprite::getWorldPosition() {
	return {};
}

void CharacterSprite::setForwardDirection() {
	this->setRotationSkewY(0);
	m_eDirection = SpriteDirection::Right;
}

void CharacterSprite::setBackwardDirection() {
	this->setRotationSkewY(180.0f);
	m_eDirection = SpriteDirection::Left;
}


void CharacterSprite::onAnimateBegin(int motionState) {
	m_iMotionState = (MotionState_t)motionState;
}

void CharacterSprite::onAnimateEnd(int motionState) {
	m_iMotionState = 0;
}

void CharacterSprite::runMotion(int motionState1) {
	for (int ePartType = 0; ePartType < CharacterPartType::Max; ++ePartType) {
		auto animate = m_vParts[ePartType]->getAnimation(motionState1);
		m_vParts[ePartType]->runSequenceOnce(
			Sequence::create(
				animate,
				CallFunc::create([=]() {animate->onAnimateEnd2(); }),		// 콜펑으로 테스트
				nullptr)
		);
	}
}

void CharacterSprite::runMotionRepeat(int motionState1) {
	for (int ePartType = 0; ePartType < CharacterPartType::Max; ++ePartType) {
		auto animate = m_vParts[ePartType]->getAnimation(motionState1);
		m_vParts[ePartType]->runSequenceRepeat(
			Sequence::create(
				animate,
				CallFunc::create([=]() {animate->onAnimateEnd2(); }),		// 콜펑으로 테스트
				nullptr)
		);
	}
	
}

void CharacterSprite::runMotion(int motionState1, int motionState2) {
	cocos2d::Vector<FiniteTimeAction*> sequences{ CharacterPartType::Max };

	for (int ePartType = 0; ePartType < CharacterPartType::Max; ++ePartType) {
		auto animate1 = m_vParts[ePartType]->getAnimation(motionState1);
		auto animate2 = m_vParts[ePartType]->getAnimation(motionState2);
		sequences.pushBack(animate1);
		sequences.pushBack(CallFunc::create([=]() {animate1->onAnimateEnd2(); }));
		sequences.pushBack(animate2);
		sequences.pushBack(CallFunc::create([=]() {animate2->onAnimateEnd2(); }));
		m_vParts[ePartType]->runSequenceOnce(Sequence::create(sequences));
		sequences.clear();
	}
}

void CharacterSprite::runMotion(int motionState1, int motionState2, int motionState3) {
	cocos2d::Vector<FiniteTimeAction*> sequences{ CharacterPartType::Max };

	for (int ePartType = 0; ePartType < CharacterPartType::Max; ++ePartType) {
		auto animate1 = m_vParts[ePartType]->getAnimation(motionState1);
		auto animate2 = m_vParts[ePartType]->getAnimation(motionState2);
		auto animate3 = m_vParts[ePartType]->getAnimation(motionState3);
		sequences.pushBack(animate1);
		sequences.pushBack(CallFunc::create([=]() {animate1->onAnimateEnd2(); }));
		sequences.pushBack(animate2);
		sequences.pushBack(CallFunc::create([=]() {animate2->onAnimateEnd2(); }));
		sequences.pushBack(animate3);
		sequences.pushBack(CallFunc::create([=]() {animate3->onAnimateEnd2(); }));
		m_vParts[ePartType]->runSequenceOnce(Sequence::create(sequences));
		sequences.clear();
	}
}

void CharacterSprite::runMotion(int motionState1, int motionState2, int motionState3, int motionState4) {
	cocos2d::Vector<FiniteTimeAction*> sequences{ CharacterPartType::Max };

	for (int ePartType = 0; ePartType < CharacterPartType::Max; ++ePartType) {
		auto animate1 = m_vParts[ePartType]->getAnimation(motionState1);
		auto animate2 = m_vParts[ePartType]->getAnimation(motionState2);
		auto animate3 = m_vParts[ePartType]->getAnimation(motionState3);
		auto animate4 = m_vParts[ePartType]->getAnimation(motionState4);
		sequences.pushBack(animate1);
		sequences.pushBack(CallFunc::create([=]() {animate1->onAnimateEnd2(); }));
		sequences.pushBack(animate2);
		sequences.pushBack(CallFunc::create([=]() {animate2->onAnimateEnd2(); }));
		sequences.pushBack(animate3);
		sequences.pushBack(CallFunc::create([=]() {animate3->onAnimateEnd2(); }));
		sequences.pushBack(animate4);
		sequences.pushBack(CallFunc::create([=]() {animate4->onAnimateEnd2(); }));
		m_vParts[ePartType]->runSequenceOnce(Sequence::create(sequences));
		sequences.clear();
	}
}


void CharacterSprite::setIdle() {
	// 코코스 백터를 인자로 받아버려서, 내가만든벡터로 전달할 수가 없네;;
	cocos2d::Vector<FiniteTimeAction*> sequences{ CharacterPartType::Max };

	for (int ePartType = 0; ePartType < CharacterPartType::Max; ++ePartType) {
		sequences.pushBack(m_vParts[ePartType]->getAnimation(MotionState::IdleBreath));
		sequences.pushBack(DelayTime::create(1.0f));
		sequences.pushBack(m_vParts[ePartType]->getAnimation(MotionState::IdleBreath));
		sequences.pushBack(DelayTime::create(1.0f));
		sequences.pushBack(m_vParts[ePartType]->getAnimation(MotionState::IdleBreath));
		sequences.pushBack(DelayTime::create(1.0f));
		sequences.pushBack(m_vParts[ePartType]->getAnimation(MotionState::IdleReload));

		m_vParts[ePartType]->runSequenceRepeat(Sequence::create(sequences));
		sequences.clear();
	}
}

void CharacterSprite::setWalk()
{
	for (int ePartType = 0; ePartType < CharacterPartType::Max; ++ePartType) {
		m_vParts[ePartType]->runSequenceRepeat(Sequence::create(m_vParts[ePartType]->getAnimation(MotionState::Walk), nullptr));
	}
}

void CharacterSprite::setRunning() {
	for (int ePartType = 0; ePartType < CharacterPartType::Max; ++ePartType) {
		m_vParts[ePartType]->runSequenceRepeat(Sequence::create(m_vParts[ePartType]->getAnimation(MotionState::Run), nullptr));
	}
}

void CharacterSprite::setSlide(SpriteDirection_t direciton) {
	stopAllActions();

	// https://home.deds.nl/~jessez/jessezoutewelle.nl/caurina/transitions/TransitionCheatSheets_2/Tweener%20-%20Transition%20cheat%20sheet%20-%20Complete.pdf
	

	for (int ePartType = 0; ePartType < CharacterPartType::Max; ++ePartType) {
		m_vParts[ePartType]->runSequenceRepeat(Sequence::create(m_vParts[ePartType]->getAnimation(MotionState::Sliding), nullptr));
	}
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