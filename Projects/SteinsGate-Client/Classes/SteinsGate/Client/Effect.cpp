/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 3:47:17 PM
 * =====================
 *
 */




#include "Tutturu.h"
#include "Effect.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/ActorSprite.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/Global.h>

#include <SteinsGate/Common/RectPoly.h>

USING_NS_JC;
USING_NS_CC;

Effect::Effect(EffectInfo* baseInfo) : m_pBaseInfo(baseInfo) {}
Effect::~Effect() {}

Effect* Effect::create(EffectInfo* baseInfo) {
	Effect* pEffect = dbg_new Effect(baseInfo);
	pEffect->initialize();
	pEffect->autorelease();
	return pEffect;
}

void Effect::initialize() {
	initActorSprite();
	initVariables();
	initListeners();
	initComponents();
}

bool Effect::initVariables() {
	Actor::initVariables();

	return true;
}

// 프로젝틸은 파츠, 애니메이션 다 1개씩임
void Effect::initActorSprite() {
	m_pActorSprite = ActorSprite::create(this, m_pBaseInfo->SpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pActorSprite);
}

void Effect::initListeners() {
}

void Effect::initComponents() {
}


void Effect::update(float delta) {
	Actor::update(delta);


	if (Global::Get()->DrawEffect) {
		m_pActorSprite->getParts().ForEach([](ActorSprite::PartData& part) { part.Part->setOpacity(255); });
	} else {
		m_pActorSprite->getParts().ForEach([](ActorSprite::PartData& part) { part.Part->setOpacity(0); });
	}
}
void Effect::onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {}
void Effect::onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {}
void Effect::onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) {}
void Effect::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) {

	// 일단 애니메이션 모두 실행 후 소멸된다고 가정
	// 추후 이펙트 추가된다면 프로젝틸 리스너처럼 이펙트 리스너로 기능 개별 구현 필요
	cleanUpAtNextFrame();
}