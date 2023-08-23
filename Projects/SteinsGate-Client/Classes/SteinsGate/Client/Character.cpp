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
#include <SteinsGate/Client/Define_Action.h>
#include <SteinsGate/Client/ActorManager.h>
#include <SteinsGate/Client/CharacterListener.h>
#include <SteinsGate/Client/MoveComponent.h>
#include <SteinsGate/Client/PhysicsComponent.h>

USING_NS_JC;
USING_NS_CC;

Character::Character() 
	: m_pBaseInfo(nullptr)
	, m_pSpriteData(nullptr)
{}

Character::Character(CharBaseInfo* charInfo, const VisualInfo& visualInfo)
	: m_pBaseInfo(charInfo)
	, m_VisualInfo(visualInfo)
	, m_pSpriteData(nullptr)
{}

Character::~Character() {
	JCORE_DELETE_SAFE(m_pSpriteData);
	_LogDebug_("캐릭터 소멸");
}

Character* Character::create(CharBaseInfo* charInfo, const VisualInfo& info) {
	Character* pCharacter = dbg_new Character(charInfo, info);
	pCharacter->initialize();
	pCharacter->autorelease();
	return pCharacter;
}

void Character::initialize() {
	initVariables();
	initThicknessBox(m_pBaseInfo->ThicknessBox);
	initActorSpriteData(m_VisualInfo);
	initActorSprite();
	initHitRecorder(32, 64);
	initListeners();
	initComponents();
}

void Character::initActorSpriteData(const VisualInfo& visualInfo) {
	DebugAssert(m_pBaseInfo != nullptr);
	JCORE_DELETE_SAFE(m_pSpriteData);
	SGVector<AnimationInfo*>& animationList = Core::DataManager->getCharAnimationInfoList(m_pBaseInfo->Code);
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

void Character::initListeners() {
	IActorListener* pListener = getListener(IActorListener::Type::eCharacter);

	if (pListener == nullptr) {
		pListener = Core::Contents.ActorListenerManager->createCharacterListener(this);
		addListener(pListener);
	}

	pListener->onCreated();
}

void Character::initComponents() {
	if (!m_Components.has(IComponent::eMove))
		m_Components.add(dbg_new MoveComponent(this));

	if (!m_Components.has(IComponent::ePhysics))
		m_Components.add(dbg_new PhysicsComponent(this));
}

CharBaseInfo* Character::getBaseInfo() {
	return m_pBaseInfo;
}

