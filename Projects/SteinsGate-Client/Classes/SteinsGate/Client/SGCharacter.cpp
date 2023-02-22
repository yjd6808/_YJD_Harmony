/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:07 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGCharacter.h"

#include <SteinsGate/Common/SGRectEx.h>
#include <SteinsGate/Common/SGRectEx.h>

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGActorSprite.h>
#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGHostPlayer.h>
#include <SteinsGate/Client/SGActionDefine.h>
#include <SteinsGate/Client/SGActorBox.h>




USING_NS_JC;
USING_NS_CC;

SGCharacter::SGCharacter() 
	: SGPhysicsActor(ActorType::Character, InvalidValue_v)
	, m_pBaseInfo(nullptr)
{}

SGCharacter::SGCharacter(int code)
	: SGPhysicsActor(ActorType::Character, code)
	, m_pBaseInfo(nullptr)
{}

SGCharacter::SGCharacter(int code, const VisualInfo& info)
	: SGPhysicsActor(ActorType::Character, code)
	, m_VisualInfo(info)
	, m_pBaseInfo(nullptr)
{}

SGCharacter::~SGCharacter() {
	_LogDebug_("캐릭터 소멸");
}

SGCharacter* SGCharacter::create(int code, const VisualInfo& info) {
	SGCharacter* pCharacter = dbg_new SGCharacter(code, info);
	pCharacter->initBaseInfo(code);
	pCharacter->initThicknessBox(pCharacter->m_pBaseInfo->ThicknessBox);
	pCharacter->initActorSprite();
	pCharacter->initHitRecorder(32, 64);
	pCharacter->autorelease();
	return pCharacter;
}

void SGCharacter::initInfo(int code, const VisualInfo& visualInfo) {
	initBaseInfo(code);
	initVisualInfo(visualInfo);
	initThicknessBox(m_pBaseInfo->ThicknessBox);
	initActorSprite();
	initHitRecorder(32, 64);
}

void SGCharacter::initVisualInfo(const VisualInfo& visualInfo) {
	DebugAssertMsg(visualInfo.Size() > 0, "유효하지 않은 비주얼 정보입니다.");
	m_VisualInfo = visualInfo;
}

void SGCharacter::initBaseInfo(int code) {
	m_iCode = code;
	SGCharBaseInfo* pBaseInfo = SGDataManager::get()->getCharInfo(code);
	m_pBaseInfo = pBaseInfo;
}

void SGCharacter::initActorSprite() {
	DebugAssertMsg(m_VisualInfo.Size() > 0, "액터 스프라이트 초기화 실패: 유효하지 않은 비주얼 정보입니다.");

	SGDataManager* pDataManager = SGDataManager::get();
	AnimationList& animationList = pDataManager->getCharAnimationInfoList(m_iCode);
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>(15, animationList.Size());

	spActorSpriteData->Parts = m_VisualInfo;
	spActorSpriteData->Animations = animationList;

	if (m_pActorSprite != nullptr) {
		m_pActorSprite->updateSpriteData(spActorSpriteData);
		return;
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
}

void SGCharacter::update(float dt) {
	SGPhysicsActor::update(dt);
}

void SGCharacter::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
}

void SGCharacter::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
}

void SGCharacter::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
}

void SGCharacter::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
}

void SGCharacter::cleanUpImmediate() {
	m_bCleanUp = true;
	m_pMapLayer = nullptr;
	SGActorBox::get()->cleanUpCharacter(this);
}

SGCharBaseInfo* SGCharacter::getBaseInfo() {
	return m_pBaseInfo;
}

