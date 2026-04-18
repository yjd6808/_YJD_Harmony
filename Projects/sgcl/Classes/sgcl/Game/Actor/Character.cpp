/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:07 AM
 * =====================
 *
 */

#include "GameCoreHeader.h"
#include "Game/Actor/Character.h"


#include "sgcl/Game/Texture/ImagePackManager.h"
#include "sgcl/Game/Actor/ActorSprite.h"
#include "sg/Util/DescLoaderMgr.h"
#include "sgcl/Game/Contents/HostPlayer.h"
#include "sgcl/Define/Define_Action.h"
#include "sgcl/Game/Contents/ActorManager.h"
#include "sgcl/Game/Listener/Character/CharacterListener.h"
#include "sgcl/Game/Component/MoveComponent.h"
#include "sgcl/Game/Component/PhysicsComponent.h"

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
	JC_DELETE_SAFE(m_pSpriteData);
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
	jc_assert(m_pBaseInfo != nullptr);
	JC_DELETE_SAFE(m_pSpriteData);
	jc::Vector<AnimationInfo*>& animationList = g_cDescMgr.GetCharAnimationInfoList(m_pBaseInfo->code_);
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
	jc_assert(m_pSpriteData != nullptr);

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
		pListener = sg::Contents.ActorListenerManager->CreateCharacterListener(this);
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
