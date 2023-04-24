/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:07 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "Character.h"

#include <SteinsGate/Common/RectEx.h>
#include <SteinsGate/Common/RectEx.h>

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/ActorSprite.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/ActionDefine.h>
#include <SteinsGate/Client/ActorBox.h>

USING_NS_JC;
USING_NS_CC;

Character::Character() 
	: PhysicsActor(ActorType::Character, InvalidValue_v)
	, m_pBaseInfo(nullptr)
{}

Character::Character(int code)
	: PhysicsActor(ActorType::Character, code)
	, m_pBaseInfo(nullptr)
{}

Character::Character(int code, const VisualInfo& info)
	: PhysicsActor(ActorType::Character, code)
	, m_VisualInfo(info)
	, m_pBaseInfo(nullptr)
{}

Character::~Character() {
	_LogDebug_("캐릭터 소멸");
}

Character* Character::create(int code, const VisualInfo& info) {
	Character* pCharacter = dbg_new Character(code, info);
	pCharacter->initBaseInfo(code);
	pCharacter->initThicknessBox(pCharacter->m_pBaseInfo->ThicknessBox);
	pCharacter->initActorSprite();
	pCharacter->initHitRecorder(32, 64);
	pCharacter->autorelease();
	return pCharacter;
}

void Character::initActorId() {
	m_iActorId = InvalidValue_v;
}

void Character::initInfo(int code, const VisualInfo& visualInfo) {
	initActorId();
	initBaseInfo(code);
	initVisualInfo(visualInfo);
	initThicknessBox(m_pBaseInfo->ThicknessBox);
	initActorSprite();
	initHitRecorder(32, 64);
}

void Character::initVisualInfo(const VisualInfo& visualInfo) {
	DebugAssertMsg(visualInfo.Size() > 0, "유효하지 않은 비주얼 정보입니다.");
	m_VisualInfo = visualInfo;
}

void Character::initBaseInfo(int code) {
	m_iCode = code;
	CharBaseInfo* pBaseInfo = DataManager::get()->getCharInfo(code);
	m_pBaseInfo = pBaseInfo;
}

void Character::initActorSprite() {
	DebugAssertMsg(m_VisualInfo.Size() > 0, "액터 스프라이트 초기화 실패: 유효하지 않은 비주얼 정보입니다.");

	DataManager* pDataManager = DataManager::get();
	AnimationList& animationList = pDataManager->getCharAnimationInfoList(m_iCode);
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>(15, animationList.Size());

	spActorSpriteData->Parts = m_VisualInfo;
	spActorSpriteData->Animations = animationList;

	if (m_pActorSprite != nullptr) {
		m_pActorSprite->updateSpriteData(spActorSpriteData);
		return;
	}

	m_pActorSprite = ActorSprite::create(this, spActorSpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pActorSprite);
}

void Character::initListener(ActorListener* listener) {
	// 캐릭터가 다양해지면 필요할듯
}


void Character::hit(const SGHitInfo& hitInfo) {
	PhysicsActor::hit(hitInfo);
}

void Character::update(float dt) {
	PhysicsActor::update(dt);
}

void Character::onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {
}

void Character::onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {
}

void Character::onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) {
}

void Character::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) {
}

void Character::cleanUpImmediate() {
	m_bCleanUp = true;
	m_pMapLayer = nullptr;
	ActorBox::get()->cleanUpCharacter(this);
}

CharBaseInfo* Character::getBaseInfo() {
	return m_pBaseInfo;
}

