/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:45 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "Monster.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/ActorSprite.h>
#include <SteinsGate/Client/AnimationDefine.h>

#include <SteinsGate/Client/MonsterAttackActivity.h>
#include <SteinsGate/Client/MonsterFallDownActivity.h>
#include <SteinsGate/Client/MonsterHitActivity.h>
#include <SteinsGate/Client/MonsterWalkActivity.h>
#include <SteinsGate/Client/MonsterIdleActivity.h>
#include <SteinsGate/Client/MonsterSitActivity.h>

USING_NS_CC;
USING_NS_JC;

Monster::Monster(MobInfo* baseInfo, AIInfo* aiInfo)
	: AIActor(ActorType::Monster, baseInfo->Code, aiInfo)
	, m_pBaseInfo(baseInfo)
{
}

Monster::~Monster() {
	for (int i = 0; i < AIActivityType::Max; ++i) {
		DeleteSafe(m_ActivityMap[i]);
	}
}

Monster* Monster::create(MobInfo* baseInfo, AIInfo* aiInfo) {
	Monster* pMonster = dbg_new Monster(baseInfo, aiInfo);

	if (pMonster && pMonster->init()) {
		pMonster->initThicknessBox(baseInfo->ThicknessBox);
		pMonster->initActorSprite();
		pMonster->initHitRecorder();		// 먼저 초기화 필요 (AIActivity에서 초기화해서 씀)
		pMonster->initAI();
		pMonster->autorelease();
		return pMonster;
	}

	return pMonster;
}

void Monster::initActorSprite() {
	DataManager* pDataManager = DataManager::Get();
	ImagePackManager* pImgPackManager = ImagePackManager::Get();
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>(m_pBaseInfo->PartsCount, m_pBaseInfo->AnimationList.Size());

	for (int i = 0; i < m_pBaseInfo->PartsCount; ++i) {
		MobPartInfo& part = m_pBaseInfo->Parts[i];
		spActorSpriteData->Parts.PushBack({ part.ZOrder, part.SgaIndex, part.ImgIndex });
	}

	for (int i = 0; i < m_pBaseInfo->AnimationList.Size(); ++i) {
		spActorSpriteData->Animations.PushBack(&m_pBaseInfo->AnimationList[i]);
	}

	m_pActorSprite = ActorSprite::create(this, spActorSpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pActorSprite);
}

void Monster::initAIActivities() {
	m_ActivityMap[AIActivityType::Walk] = dbg_new MonsterWalkActivity(this);
	m_ActivityMap[AIActivityType::Idle] = dbg_new MonsterIdleActivity(this);
	m_ActivityMap[AIActivityType::Attack] = dbg_new MonsterAttackActivity(this);
	m_ActivityMap[AIActivityType::Hit] = dbg_new MonsterHitActivity(this);
	m_ActivityMap[AIActivityType::FallDown] = dbg_new MonsterFallDownActivity(this);
	m_ActivityMap[AIActivityType::Sit] = dbg_new MonsterSitActivity(this);

	runActivity(m_ActivityMap[AIActivityType::Idle]);	// 아무것도 설정안하면 기본 실행 시간 1초
	
}

void Monster::initListener(ActorListener* listener) {
	DebugAssertMsg(m_pListener == nullptr, "이미 액터 리스너가 초기화 되어있습니다.");
	DebugAssertMsg(listener->getActorType() == ActorType::Monster, "몬스터 리스너만 초기화 가능합니다.");
	m_pListener = listener;
}


void Monster::hit(const SGHitInfo& hitInfo) {
	AIActor::hit(hitInfo);

	if (hitInfo.AttackDataInfo->IsFallDownAttack) {
		runActivity(AIActivityType::FallDown);
		return;
	}

	runActivity(AIActivityType::Hit);
}


void Monster::update(float dt) {
	AIActor::update(dt);
}

void Monster::initWanderAcitivity(AIActivity* wanderActivity) {
	
	if (wanderActivity->getType() == AIActivityType::Walk) {
		MonsterWalkActivity* pWalkActivity = (MonsterWalkActivity*)wanderActivity;
		float fTime = SGRandom::random_real(m_pAiInfo->WanderWalkTime[0], m_pAiInfo->WanderWalkTime[1]);
		pWalkActivity->setLimit(fTime);
		pWalkActivity->setMode(MonsterWalkActivity::eWander);
		pWalkActivity->setDestination(getRandomSightPos());

	} else if (wanderActivity->getType() == AIActivityType::Idle) {
		MonsterIdleActivity* pIdleActivity = (MonsterIdleActivity*)wanderActivity;
		float fTime = SGRandom::random_real(m_pAiInfo->IdleTime[0], m_pAiInfo->IdleTime[1]);
		pIdleActivity->setLimit(fTime);
	}
}

void Monster::initTrackAcitivity(AIActivity* trackActivity) {
	if (trackActivity->getType() == AIActivityType::Walk) {
		MonsterWalkActivity* pWalkActivity = (MonsterWalkActivity*)trackActivity;
		float fTime = SGRandom::random_real(m_pAiInfo->FollowWalkTime[0], m_pAiInfo->FollowWalkTime[1]);
		pWalkActivity->setLimit(fTime);
		pWalkActivity->setMode(MonsterWalkActivity::eTrack);
		pWalkActivity->setTarget(getTarget());
	}
}

void Monster::initAngryAcitivity(AIActivity* angryActivity) {
	// 공격은 그냥 실행하면 되서 딱히 없는 듯
}

void Monster::onSelectedActivity(AIActivity* selectedActivity) {
	switch (m_eActivityState) {
	case eWander: {
		initWanderAcitivity(selectedActivity);
		break;
	}
	case eTrack: {
		initTrackAcitivity(selectedActivity);
		break;
	}
	case eAngry: {
		initAngryAcitivity(selectedActivity);
		break;
	}
	default: DebugAssertMsg(false, "상태 타입이 이상합니다.");
	}

}

void Monster::onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {
	m_pRunningActivity->onFrameBegin(animation, texture);
}

void Monster::onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	m_pRunningActivity->onFrameEnd(animation, texture);
}

void Monster::onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) {
	m_pRunningActivity->onAnimationBegin(animation, texture);
	
}

void Monster::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	m_pRunningActivity->onAnimationEnd(animation, texture);
}

MobInfo* Monster::getBaseInfo() {
	return m_pBaseInfo;
}