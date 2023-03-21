﻿/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 3:47:17 PM
 * =====================
 *
 */




#include "Tutturu.h"
#include "SGEffect.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGActorSprite.h>
#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGHostPlayer.h>
#include <SteinsGate/Client/SGGlobal.h>

#include <SteinsGate/Common/RectPoly.h>

USING_NS_JC;
USING_NS_CC;

SGEffect::SGEffect(SGEffectInfo* baseInfo)
	: SGActor(ActorType::Effect, baseInfo->Code)
	, m_pBaseInfo(baseInfo)
{}

SGEffect::~SGEffect() {
}



SGEffect* SGEffect::create(SGEffectInfo* baseInfo) {
	SGEffect* pEffect = dbg_new SGEffect(baseInfo);

	pEffect->initActorSprite();
	pEffect->initVariables();
	pEffect->autorelease();

	return pEffect;
}

bool SGEffect::initVariables() {
	SGActor::initVariables();

	return true;
}


void SGEffect::initListener(SGActorListener* listener) {
	DebugAssertMsg(m_pListener == nullptr, "이미 액터 리스너가 초기화 되어있습니다.");
	DebugAssertMsg(listener->getActorType() == ActorType::Effect, "이펙트 리스너만 초기화 가능합니다.");
	m_pListener = listener;
	m_pListener->injectActor(this);
}



// 프로젝틸은 파츠, 애니메이션 다 1개씩임
void SGEffect::initActorSprite() {
	SGDataManager* pDataManager = SGDataManager::get();
	SGImagePackManager* pImgPackManager = SGImagePackManager::get();
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>(1, 1);	// 프로젝틸도 파츠, 애니메이션 모두 한개

	spActorSpriteData->Parts.PushBack({ 0, m_pBaseInfo->SgaIndex, m_pBaseInfo->ImgIndex });
	spActorSpriteData->Animations.PushBack(&m_pBaseInfo->Animation);

	m_pActorSprite = SGActorSprite::create(this, spActorSpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pActorSprite);
}


void SGEffect::update(float delta) {
	SGActor::update(delta);


	if (SGGlobal::get()->DrawEffect) {
		m_pActorSprite->getParts().Extension().ForEach([](SGActorSprite::PartData& part) { part.Part->setOpacity(255); });
	} else {
		m_pActorSprite->getParts().Extension().ForEach([](SGActorSprite::PartData& part) { part.Part->setOpacity(0); });
	}
}
void SGEffect::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {}
void SGEffect::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {}
void SGEffect::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {}
void SGEffect::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {

	// 일단 애니메이션 모두 실행 후 소멸된다고 가정
	// 추후 이펙트 추가된다면 프로젝틸 리스너처럼 이펙트 리스너로 기능 개별 구현 필요
	cleanUpNext();
}