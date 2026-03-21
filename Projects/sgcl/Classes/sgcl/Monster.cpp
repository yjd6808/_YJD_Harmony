/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:45 AM
 * =====================
 *
 */

#include "GameCoreHeader.h"
#include "Monster.h"

#include "sg/_Struct/SteinsGate_AttackData.h"
#include "sgcl/MoveComponent.h"
#include "sgcl/PhysicsComponent.h"
#include "sgcl/AIComponent.h"

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
Monster::Monster(MonsterInfo* _pBaseInfo)
: pBaseInfo_(_pBaseInfo)
, pStatInfo_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Monster::~Monster()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Monster* Monster::Create(MonsterInfo* _pBaseInfo)
{
	Monster* pMonster = dbg_new Monster(_pBaseInfo);
	pMonster->Initialize();
	pMonster->autorelease();
	return pMonster;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Monster::Initialize()
{
	InitVariables();
	InitThicknessBox(pBaseInfo_->thicknessBox_);
	InitActorSprite();
	InitHitRecorder(); // 먼저 초기화 필요 (AIActivity에서 초기화해서 씀)
	InitListeners();
	InitComponents();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Monster::InitActorSprite()
{
	if (!pActorSprite_)
	{
		jc_assert(pBaseInfo_->pSpriteData_ != nullptr);
		pActorSprite_ = ActorSprite::Create(this, pBaseInfo_->pSpriteData_);
		pActorSprite_->setAnchorPoint(Vec2::ZERO);
		pActorSprite_->RunAnimation(1);
		this->addChild(pActorSprite_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Monster::InitListeners()
{
	IActorListener* pListener = GetListener(IActorListener::Type::eMonster);

	if (pListener == nullptr)
	{
		pListener = sg::Contents.ActorListenerManager->CreateMonsterListener(this);
		AddListener(pListener);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Monster::InitComponents()
{
	if (!components_.Has(IComponent::eMove))
	{
		components_.Add(dbg_new MoveComponent(this));
	}

	if (!components_.Has(IComponent::ePhysics))
	{
		components_.Add(dbg_new PhysicsComponent(this));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Monster::Hit(const HitInfo& _hitInfo)
{
	Actor::Hit(_hitInfo);
	AIComponent* pAIComponent = components_.Get<AIComponent>();

	if (pAIComponent == nullptr)
	{
		return;
	}

	if (_hitInfo.pAttackDataInfo_->isFallDownAttack_)
	{
		pAIComponent->RunActivity(AIActivityType::FallDown);
		return;
	}

	pAIComponent->RunActivity(AIActivityType::Hit);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Monster::SetStatInfo(MonsterStatInfo* _pStatInfo)
{
	pStatInfo_ = _pStatInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
MonsterInfo* Monster::GetBaseInfo()
{
	return pBaseInfo_;
}

//////////////////////////////////////////////////////////////////////////////////////////
MonsterStatInfo* Monster::GetStatInfo()
{
	return pStatInfo_;
}
