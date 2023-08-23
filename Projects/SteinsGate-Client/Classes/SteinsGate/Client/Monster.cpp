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

Monster::Monster(MonsterInfo* baseInfo)
	: m_pBaseInfo(baseInfo)
	, m_pStatInfo(nullptr)
{}

Monster::~Monster() {}

Monster* Monster::create(MonsterInfo* baseInfo) {
	Monster* pMonster = dbg_new Monster(baseInfo);
	pMonster->initialize();
	pMonster->autorelease();
	return pMonster;
}

void Monster::initialize() {
	initVariables();
	initThicknessBox(m_pBaseInfo->ThicknessBox);
	initActorSprite();
	initHitRecorder();		// 먼저 초기화 필요 (AIActivity에서 초기화해서 씀)
	initListeners();
	initComponents();
}

void Monster::initActorSprite() {
	if (!m_pActorSprite) {
		DebugAssert(m_pBaseInfo->SpriteData != nullptr);
		m_pActorSprite = ActorSprite::create(this, m_pBaseInfo->SpriteData);
		m_pActorSprite->setAnchorPoint(Vec2::ZERO);
		m_pActorSprite->runAnimation(1);
		this->addChild(m_pActorSprite);
	}
}

void Monster::initListeners() {
	IActorListener* pListener = getListener(IActorListener::Type::eMonster);

	if (pListener == nullptr) {
		pListener = Core::Contents.ActorListenerManager->createMonsterListener(this);
		addListener(pListener);
	}
}

void Monster::initComponents() {
	if (!m_Components.has(IComponent::eMove))
		m_Components.add(dbg_new MoveComponent(this));

	if (!m_Components.has(IComponent::ePhysics))
		m_Components.add(dbg_new PhysicsComponent(this));
}

void Monster::hit(const HitInfo& hitInfo) {
	Actor::hit(hitInfo);
	AIComponent* pAIComponent = m_Components.get<AIComponent>();

	if (pAIComponent == nullptr) {
		return;
	}

	if (hitInfo.AttackDataInfo->IsFallDownAttack) {
		pAIComponent->runActivity(AIActivityType::FallDown);
		return;
	}

	pAIComponent->runActivity(AIActivityType::Hit);
}


void Monster::setStatInfo(MonsterStatInfo* statInfo) {
	m_pStatInfo = statInfo;
}

MonsterInfo* Monster::getBaseInfo() {
	return m_pBaseInfo;
}

MonsterStatInfo* Monster::getStatInfo() {
	return m_pStatInfo;
}
