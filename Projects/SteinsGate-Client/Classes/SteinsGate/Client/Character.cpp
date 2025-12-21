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

//////////////////////////////////////////////////////////////////////////////////////////
Character::Character()
: m_pBaseInfo(nullptr)
, m_pSpriteData(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Character::Character(CharBaseInfo* _pCharInfo, const VisualInfo& _visualInfo)
: m_pBaseInfo(_pCharInfo)
, m_VisualInfo(_visualInfo)
, m_pSpriteData(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Character::~Character()
{
	JCORE_DELETE_SAFE(m_pSpriteData);
	_LogDebug_("캐릭터 소멸");
}

//////////////////////////////////////////////////////////////////////////////////////////
Character* Character::Create(CharBaseInfo* _pCharInfo, const VisualInfo& _visualInfo)
{
	Character* pCharacter = dbg_new Character(_pCharInfo, _visualInfo);
	pCharacter->Initialize();
	pCharacter->autorelease();
	return pCharacter;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Character::Initialize()
{
	InitVariables();
	InitThicknessBox(m_pBaseInfo->thicknessBox_);
	InitActorSpriteData(m_VisualInfo);
	InitActorSprite();
	InitHitRecorder(32, 64);
	InitListeners();
	InitComponents();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Character::InitActorSpriteData(const VisualInfo& _visualInfo)
{
	DebugAssert(m_pBaseInfo != nullptr);
	JCORE_DELETE_SAFE(m_pSpriteData);
	SGVector<AnimationInfo*>& animationList = Core::DataManager->GetCharAnimationInfoList(m_pBaseInfo->code_);
	m_pSpriteData = dbg_new ActorSpriteData(ActorPartSpritePositioningRule::InFrameSize, 15, animationList.Size());
	m_pSpriteData->parts_ = _visualInfo;
	m_pSpriteData->parts_.Sort(
		[](ActorPartSpriteData& lhs, ActorPartSpriteData& rhs) { return lhs.zOrder_ < rhs.zOrder_; });
	animationList.ForEach([this](AnimationInfo* _pAnimationInfo)
	{
		m_pSpriteData->animations_.PushBack(*_pAnimationInfo);
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
void Character::InitActorSprite()
{
	DebugAssert(m_pSpriteData != nullptr);

	if (pActorSprite_ != nullptr)
	{
		pActorSprite_->UpdateSpriteData(m_pSpriteData);
		return;
	}

	pActorSprite_ = ActorSprite::Create(this, m_pSpriteData);
	pActorSprite_->setAnchorPoint(Vec2::ZERO);
	this->addChild(pActorSprite_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Character::InitListeners()
{
	IActorListener* pListener = GetListener(IActorListener::Type::eCharacter);

	if (pListener == nullptr)
	{
		pListener = Core::Contents.ActorListenerManager->CreateCharacterListener(this);
		AddListener(pListener);
	}

	pListener->OnCreated();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Character::InitComponents()
{
	if (!components_.Has(IComponent::eMove))
		components_.Add(dbg_new MoveComponent(this));

	if (!components_.Has(IComponent::ePhysics))
		components_.Add(dbg_new PhysicsComponent(this));
}

//////////////////////////////////////////////////////////////////////////////////////////
CharBaseInfo* Character::GetBaseInfo()
{
	return m_pBaseInfo;
}
