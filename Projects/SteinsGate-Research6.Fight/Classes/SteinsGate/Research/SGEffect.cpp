/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 3:47:17 PM
 * =====================
 *
 */




#include "Tutturu.h"
#include "SGEffect.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGActorSprite.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGPlayer.h>

#include <SteinsGate/Common/Engine/RectPoly.h>

USING_NS_JC;
USING_NS_CC;

SGEffect::SGEffect(SGEffectInfo* baseInfo)
	: SGActor(ActorType::Effect, baseInfo->Code)
	, m_pBaseInfo(baseInfo)
{}

SGEffect::~SGEffect() {
}



SGEffect* SGEffect::create(SGEffectInfo* baseInfo) {
	SGEffect* pEffect = new SGEffect(baseInfo);

	if (pEffect && pEffect->init()) {
		pEffect->initActorSprite();
		pEffect->autorelease();
		return pEffect;
	}

	return pEffect;
}

bool SGEffect::init() {
	SGActor::init();


	return true;
}


void SGEffect::initListener(SGActorListener* listener) {
	DebugAssertMessage(m_pListener == nullptr, "이미 액터 리스너가 초기화 되어있습니다.");
	DebugAssertMessage(listener->getActorType() == ActorType::Effect, "이펙트 리스너만 초기화 가능합니다.");
	m_pListener = listener;
	m_pListener->injectActor(this);
}



// 프로젝틸은 파츠, 애니메이션 다 1개씩임
void SGEffect::initActorSprite() {
	SGDataManager* pDataManager = SGDataManager::getInstance();
	SGImagePackManager* pImgPackManager = SGImagePackManager::getInstance();
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>(1, 1);	// 프로젝틸도 파츠, 애니메이션 모두 한개

	spActorSpriteData->Parts.PushBack({ 0, m_pBaseInfo->NpkIndex, m_pBaseInfo->ImgIndex });
	spActorSpriteData->Animations.PushBack(&m_pBaseInfo->Animation);

	m_pActorSprite = SGActorSprite::create(this, spActorSpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	m_pActorSprite->runAnimation(1);
	this->addChild(m_pActorSprite);
}


void SGEffect::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {}
void SGEffect::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {}
void SGEffect::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {}
void SGEffect::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {

	// 일단 애니메이션 모두 실행 후 소멸된다고 가정
	// 추후 이펙트 추가된다면 프로젝틸 리스너처럼 이펙트 리스너로 기능 개별 구현 필요
	registerCleanUp();
}