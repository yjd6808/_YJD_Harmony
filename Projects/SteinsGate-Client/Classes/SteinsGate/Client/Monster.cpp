/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:45 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "Monster.h"

#include <SteinsGate/Client/AttackDataInfo.h>
#include <SteinsGate/Client/MoveComponent.h>
#include <SteinsGate/Client/PhysicsComponent.h>
#include <SteinsGate/Client/AIComponent.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
Monster::Monster(MonsterInfo* _pBaseInfo)
: baseInfo_(_pBaseInfo)
, statInfo_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Monster::~Monster()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Monster* Monster::create(MonsterInfo* _pBaseInfo)
{
	Monster* pMonster = dbg_new Monster(_pBaseInfo);
	pMonster->initialize();
	pMonster->autorelease();
	return pMonster;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Monster::initialize()
{
	initVariables();
	initThicknessBox(baseInfo_->thicknessBox_);
	initActorSprite();
	initHitRecorder(); // 먼저 초기화 필요 (AIActivity에서 초기화해서 씀)
	initListeners();
	initComponents();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Monster::initActorSprite()
{
	if (!m_pActorSprite)
	{
		DebugAssert(baseInfo_->pSpriteData_ != nullptr);
		m_pActorSprite = ActorSprite::create(this, baseInfo_->pSpriteData_);
		m_pActorSprite->setAnchorPoint(Vec2::ZERO);
		m_pActorSprite->runAnimation(1);
		this->addChild(m_pActorSprite);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Monster::initListeners()
{
	IActorListener* pListener = getListener(IActorListener::Type::eMonster);

	if (pListener == nullptr)
	{
		pListener = Core::Contents.ActorListenerManager->createMonsterListener(this);
		addListener(pListener);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Monster::initComponents()
{
	if (!m_Components.has(IComponent::eMove))
	{
		m_Components.add(dbg_new MoveComponent(this));
	}

	if (!m_Components.has(IComponent::ePhysics))
	{
		m_Components.add(dbg_new PhysicsComponent(this));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Monster::hit(const HitInfo& _hitInfo)
{
	Actor::hit(_hitInfo);
	AIComponent* pAIComponent = m_Components.get<AIComponent>();

	if (pAIComponent == nullptr)
	{
		return;
	}

	if (_hitInfo.AttackDataInfo->isFallDownAttack_)
	{
		pAIComponent->runActivity(AIActivityType::FallDown);
		return;
	}

	pAIComponent->runActivity(AIActivityType::Hit);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Monster::setStatInfo(MonsterStatInfo* _pStatInfo)
{
	statInfo_ = _pStatInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
MonsterInfo* Monster::getBaseInfo()
{
	return baseInfo_;
}

//////////////////////////////////////////////////////////////////////////////////////////
MonsterStatInfo* Monster::getStatInfo()
{
	return statInfo_;
}
