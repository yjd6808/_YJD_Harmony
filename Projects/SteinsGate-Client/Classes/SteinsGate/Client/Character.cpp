/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:07 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "Character.h"

#include <SteinsGate/Common/RectEx.h>
#include <SteinsGate/Common/RectEx.h>

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/ActorSprite.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/ActionDefine.h>
#include <SteinsGate/Client/ActorBox.h>
#include <SteinsGate/Client/CharacterListener.h>
#include <SteinsGate/Client/MoveComponent.h>

USING_NS_JC;
USING_NS_CC;

Character::Character() 
	: PhysicsActor(ActorType::Character, InvalidValue_v)
	, m_pBaseInfo(nullptr)
	, m_pSpriteData(nullptr)
{}

Character::Character(int code)
	: PhysicsActor(ActorType::Character, code)
	, m_pBaseInfo(nullptr)
	, m_pSpriteData(nullptr)
{}

Character::~Character() {
	JCORE_DELETE_SAFE(m_pSpriteData);
	_LogDebug_("캐릭터 소멸");
}

Character* Character::create(int code, const VisualInfo& info) {
	Character* pCharacter = dbg_new Character(code);
	pCharacter->initInfo(code, info);
	pCharacter->autorelease();
	return pCharacter;
}

void Character::initActorId() {
	m_iActorId = InvalidValue_v;
}

void Character::initInfo(int code, const VisualInfo& visualInfo) {
	initVariables();
	initActorId();
	initBaseInfo(code);
	initThicknessBox(m_pBaseInfo->ThicknessBox);
	initActorSpriteData(visualInfo);
	initActorSprite();
	initHitRecorder(32, 64);
	initListener();
	initComponents();
}

void Character::initBaseInfo(int code) {
	CharBaseInfo* pBaseInfo = DataManager::Get()->getCharInfo(code);
	m_pBaseInfo = pBaseInfo;
}

void Character::initActorSpriteData(const VisualInfo& visualInfo) {
	DebugAssert(m_pBaseInfo != nullptr);
	JCORE_DELETE_SAFE(m_pSpriteData);
	SGVector<AnimationInfo*>& animationList = CoreDataManager_v->getCharAnimationInfoList(m_pBaseInfo->Code);
	m_pSpriteData = dbg_new ActorSpriteData(ActorPartSpritePositioningRule::InFrameSize, 15, animationList.Size());
	m_pSpriteData->Parts = visualInfo;
	m_pSpriteData->Parts.Sort([](ActorPartSpriteData& lhs, ActorPartSpriteData& rhs) { return lhs.ZOrder < rhs.ZOrder; });
	animationList.ForEach([this](AnimationInfo* animationInfo) { m_pSpriteData->Animations.PushBack(*animationInfo); });
}

void Character::initActorSprite() {
	DebugAssert(m_pSpriteData != nullptr);

	if (m_pActorSprite != nullptr) {
		m_pActorSprite->updateSpriteData(m_pSpriteData);
		return;
	}

	m_pActorSprite = ActorSprite::create(this, m_pSpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pActorSprite);
}

void Character::initListener() {
	ActorListener* pListener = getListener(ActorListener::Type::eCharacter);

	if (pListener == nullptr) {
		pListener = CoreActorListenerManager_v->createCharacterListener((CharType_t)m_pBaseInfo->Code);
		pListener->injectActor(this);
		addListener(pListener);
	}

	pListener->onCreated();
}

void Character::initComponents() {
	Actor::initComponents();

	m_Components.add(dbg_new MoveComponent(this));
}

void Character::hit(const HitInfo& hitInfo) {
	PhysicsActor::hit(hitInfo);
}

void Character::update(float dt) {
	PhysicsActor::update(dt);
}

void Character::onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {
	PhysicsActor::onFrameBegin(animation, texture);
}

void Character::onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	PhysicsActor::onFrameEnd(animation, texture);
}

void Character::onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) {
	PhysicsActor::onAnimationBegin(animation, texture);
}

void Character::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	PhysicsActor::onAnimationEnd(animation, texture);
}

CharBaseInfo* Character::getBaseInfo() {
	return m_pBaseInfo;
}

