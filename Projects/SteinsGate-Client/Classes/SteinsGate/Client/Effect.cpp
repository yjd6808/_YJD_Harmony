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

Effect::Effect(EffectInfo* baseInfo)
	: Actor(ActorType::Effect, baseInfo->Code)
	, m_pBaseInfo(baseInfo)
{}

Effect::~Effect() {
}



Effect* Effect::create(EffectInfo* baseInfo) {
	Effect* pEffect = dbg_new Effect(baseInfo);

	pEffect->initActorSprite();
	pEffect->initVariables();
	pEffect->autorelease();

	return pEffect;
}

bool Effect::initVariables() {
	Actor::initVariables();

	return true;
}


void Effect::initListener(ActorListener* listener) {
	DebugAssertMsg(m_pListener == nullptr, "이미 액터 리스너가 초기화 되어있습니다.");
	DebugAssertMsg(listener->getActorType() == ActorType::Effect, "이펙트 리스너만 초기화 가능합니다.");
	m_pListener = listener;
	m_pListener->injectActor(this);
}



// 프로젝틸은 파츠, 애니메이션 다 1개씩임
void Effect::initActorSprite() {
	DataManager* pDataManager = DataManager::Get();
	ImagePackManager* pImgPackManager = ImagePackManager::Get();
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>(1, 1);	// 프로젝틸도 파츠, 애니메이션 모두 한개

	spActorSpriteData->Parts.PushBack({ 0, m_pBaseInfo->SgaIndex, m_pBaseInfo->ImgIndex });
	spActorSpriteData->Animations.PushBack(&m_pBaseInfo->Animation);

	m_pActorSprite = ActorSprite::create(this, spActorSpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pActorSprite);
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
	cleanUpNext();
}