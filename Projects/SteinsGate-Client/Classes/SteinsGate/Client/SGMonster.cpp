/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:45 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGMonster.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGActorSprite.h>
#include <SteinsGate/Client/SGAnimationDefine.h>

#include <SteinsGate/Client/SGMonsterAttackActivity.h>
#include <SteinsGate/Client/SGMonsterFallDownActivity.h>
#include <SteinsGate/Client/SGMonsterHitActivity.h>
#include <SteinsGate/Client/SGMonsterWalkActivity.h>
#include <SteinsGate/Client/SGMonsterIdleActivity.h>
#include <SteinsGate/Client/SGMonsterSitActivity.h>

USING_NS_CC;
USING_NS_JC;

SGMonster::SGMonster(SGMobInfo* baseInfo, SGAIInfo* aiInfo)
	: SGAIActor(ActorType::Monster, baseInfo->Code, aiInfo)
	, m_pBaseInfo(baseInfo)
{
}

SGMonster::~SGMonster() {
	for (int i = 0; i < AIActivity::Max; ++i) {
		DeleteSafe(m_ActivityMap[i]);
	}
}

SGMonster* SGMonster::create(SGMobInfo* baseInfo, SGAIInfo* aiInfo) {
	SGMonster* pMonster = dbg_new SGMonster(baseInfo, aiInfo);

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

void SGMonster::initActorSprite() {
	SGDataManager* pDataManager = SGDataManager::get();
	SGImagePackManager* pImgPackManager = SGImagePackManager::get();
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>(m_pBaseInfo->PartsCount, m_pBaseInfo->AnimationList.Size());

	for (int i = 0; i < m_pBaseInfo->PartsCount; ++i) {
		SGMobPartInfo& part = m_pBaseInfo->Parts[i];
		spActorSpriteData->Parts.PushBack({ part.ZOrder, part.SgaIndex, part.ImgIndex });
	}

	for (int i = 0; i < m_pBaseInfo->AnimationList.Size(); ++i) {
		spActorSpriteData->Animations.PushBack(&m_pBaseInfo->AnimationList[i]);
	}

	m_pActorSprite = SGActorSprite::create(this, spActorSpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pActorSprite);
}

void SGMonster::initAIActivities() {
	m_ActivityMap[AIActivity::Walk] = dbg_new SGMonsterWalkActivity(this);
	m_ActivityMap[AIActivity::Idle] = dbg_new SGMonsterIdleActivity(this);
	m_ActivityMap[AIActivity::Attack] = dbg_new SGMonsterAttackActivity(this);
	m_ActivityMap[AIActivity::Hit] = dbg_new SGMonsterHitActivity(this);
	m_ActivityMap[AIActivity::FallDown] = dbg_new SGMonsterFallDownActivity(this);
	m_ActivityMap[AIActivity::Sit] = dbg_new SGMonsterSitActivity(this);

	runActivity(m_ActivityMap[AIActivity::Idle]);	// 아무것도 설정안하면 기본 실행 시간 1초
	
}

void SGMonster::initListener(SGActorListener* listener) {
	DebugAssertMsg(m_pListener == nullptr, "이미 액터 리스너가 초기화 되어있습니다.");
	DebugAssertMsg(listener->getActorType() == ActorType::Monster, "몬스터 리스너만 초기화 가능합니다.");
	m_pListener = listener;
}


void SGMonster::hit(const SGHitInfo& hitInfo) {
	SGAIActor::hit(hitInfo);

	if (hitInfo.AttackDataInfo->IsFallDownAttack) {
		runActivity(AIActivity::FallDown);
		return;
	}

	runActivity(AIActivity::Hit);
}


void SGMonster::update(float dt) {
	SGAIActor::update(dt);
}

void SGMonster::initWanderAcitivity(SGAIActivity* wanderActivity) {
	
	if (wanderActivity->getType() == AIActivity::Walk) {
		SGMonsterWalkActivity* pWalkActivity = (SGMonsterWalkActivity*)wanderActivity;
		float fTime = SGRandom::random_real(m_pAiInfo->WanderWalkTime[0], m_pAiInfo->WanderWalkTime[1]);
		pWalkActivity->setLimit(fTime);
		pWalkActivity->setMode(SGMonsterWalkActivity::eWander);
		pWalkActivity->setDestination(getRandomSightPos());

	} else if (wanderActivity->getType() == AIActivity::Idle) {
		SGMonsterIdleActivity* pIdleActivity = (SGMonsterIdleActivity*)wanderActivity;
		float fTime = SGRandom::random_real(m_pAiInfo->IdleTime[0], m_pAiInfo->IdleTime[1]);
		pIdleActivity->setLimit(fTime);
	}
}

void SGMonster::initTrackAcitivity(SGAIActivity* trackActivity) {
	if (trackActivity->getType() == AIActivity::Walk) {
		SGMonsterWalkActivity* pWalkActivity = (SGMonsterWalkActivity*)trackActivity;
		float fTime = SGRandom::random_real(m_pAiInfo->FollowWalkTime[0], m_pAiInfo->FollowWalkTime[1]);
		pWalkActivity->setLimit(fTime);
		pWalkActivity->setMode(SGMonsterWalkActivity::eTrack);
		pWalkActivity->setTarget(getTarget());
	}
}

void SGMonster::initAngryAcitivity(SGAIActivity* angryActivity) {
	// 공격은 그냥 실행하면 되서 딱히 없는 듯
}

void SGMonster::onSelectedActivity(SGAIActivity* selectedActivity) {
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

void SGMonster::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	m_pRunningActivity->onFrameBegin(animation, texture);
}

void SGMonster::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	m_pRunningActivity->onFrameEnd(animation, texture);
}

void SGMonster::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	m_pRunningActivity->onAnimationBegin(animation, texture);
	
}

void SGMonster::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	m_pRunningActivity->onAnimationEnd(animation, texture);
}

SGMobInfo* SGMonster::getBaseInfo() {
	return m_pBaseInfo;
}