/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 10:08:14 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "AIComponent.h"

#include <SteinsGate/Client/MapLayer.h>
#include <SteinsGate/Client/SGVec2Ex.h>

USING_NS_JC;
USING_NS_CC;

#define SightRandomPosMinAlpha 0.2f
#define EnableLog false

#if EnableLog
	#define AILog(fmt, ...) Log(fmt, ##__VA_ARGS__)
#else
	#define AILog(...)
#endif

AIComponent::AIComponent(Actor* actor)
	: ActorComponent(actor)
	, m_pAIInfo(nullptr)
	, m_pTarget(nullptr)
	, m_ActivityMap{}
	, m_pRunningActivity()
	, m_eState(AIState::None)
	, m_eActivityState(AIState::None)
	, m_ePreviousState(AIState::None)
{}

AIComponent::~AIComponent() {
	Arrays::ForEachDelete(m_ActivityMap);
}

void AIComponent::initialize() {
	m_pRunningActivity = nullptr;
	m_eState = AIState::Wander;
	m_ePreviousState = AIState::None;
	m_pTarget = nullptr;
}

void AIComponent::onUpdate(float dt) {

	if (m_pAIInfo == nullptr)
		return;

	updateState();				// 변경가능한 상태 확인
	selectActivity();			// 해당 상태에서 수행가능한 액티비티 설정
	updateDirection();
	updateActivity(dt);			// 액티비티 지속 업데이트
}

void AIComponent::setAIInfo(AIInfo* info) {
	m_pAIInfo = info;
}

void AIComponent::setAttackActivity(AttackActivity* attackActivity) {
	DebugAssert(m_ActivityMap[AIActivityType::Attack] == nullptr);
	m_ActivityMap[AIActivityType::Attack] = attackActivity;
}

void AIComponent::setFallDownActivity(FallDownActivity* fallDownActivity) {
	DebugAssert(m_ActivityMap[AIActivityType::FallDown] == nullptr);
	m_ActivityMap[AIActivityType::FallDown] = fallDownActivity;
}

void AIComponent::setHitActivity(HitActivity* hitActivity) {
	DebugAssert(m_ActivityMap[AIActivityType::Hit] == nullptr);
	m_ActivityMap[AIActivityType::Hit] = hitActivity;
}

void AIComponent::setIdleActivity(IdleActivity* idleActivity) {
	DebugAssert(m_ActivityMap[AIActivityType::Idle] == nullptr);
	m_ActivityMap[AIActivityType::Idle] = idleActivity;
}

void AIComponent::setWalkActivity(WalkActivity* walkActivity) {
	DebugAssert(m_ActivityMap[AIActivityType::Walk] == nullptr);
	m_ActivityMap[AIActivityType::Walk] = walkActivity;
}

void AIComponent::setSitActivity(SitActivity* sitActivity) {
	DebugAssert(m_ActivityMap[AIActivityType::Sit] == nullptr);
	m_ActivityMap[AIActivityType::Sit] = sitActivity;
}

void AIComponent::updateState() {
	MapLayer* pMapLayer = m_pActor->getMapLayer();

	if (pMapLayer == nullptr) {
		return;
	}

	if (m_pRunningActivity && m_pRunningActivity->isRunning()) {
		return;
	}

	m_ePreviousState = m_eState;
	float enemyDist;
	Actor* pPreviousTarget = m_pTarget;
	m_pTarget = pMapLayer->findNearestCharacterInRadious(m_pActor, m_pAIInfo->SightRadious, enemyDist);

	if (m_pTarget == nullptr) {
		AILog("[상태] 적 미발견: 배회 상태\n");
		m_eState = AIState::Wander;
		return;
	}

	// TODO: 타겟 변경 알려줘야할까?
	if (pPreviousTarget == nullptr) {
	}

	if (enemyDist > m_pAIInfo->AttackRadious) {
		AILog("[상태] 적 발견: 추격 상태\n");
		m_eState = AIState::Track;
		return;
	}

	AILog("[상태] 적 근접: 분노 상태\n");
	m_eState = AIState::Angry;
}



void AIComponent::selectActivity() {
	if (m_pRunningActivity && m_pRunningActivity->isRunning())
		return;

	switch (m_eState) {
	case AIState::Wander:	selectWanderActivity();		break;
	case AIState::Track:	selectTrackActivity();		break;
	case AIState::Angry:	selectAngryActivity();		break;
	}

	if (m_pRunningActivity == nullptr)
		return;

	m_pRunningActivity->onActivitySelectFromAIRoutine(m_pAIInfo, m_eActivityState);
	m_pRunningActivity->run();
}

void AIComponent::updateActivity(float dt) {
	if (m_pRunningActivity == nullptr)
		return;

	m_pRunningActivity->onUpdate(dt);
}

void AIComponent::updateDirection() {
	if (m_pRunningActivity == nullptr)
		return;

	AIActivityType_t eActivityType = m_pRunningActivity->getType();

	if (eActivityType != AIActivityType::Idle && 
		eActivityType != AIActivityType::Walk) {
		return;
	}

	if (m_eState >= AIState::Track && m_pTarget) {
		SpriteDirection_t eWhereIsTarget;
		SGVec2Ex::getLookDirectionX(m_pActor->getPositionRealCenterX(), m_pTarget->getPositionRealCenterX(), eWhereIsTarget);
		m_pActor->setSpriteDirection(eWhereIsTarget);
	}
}

void AIComponent::selectWanderActivity() {
	const float fRnd = SGRandom::random_real(0.0f, 100.0f);
	m_eActivityState = AIState::Wander;

	if (fRnd < m_pAIInfo->WanderProbs[AIWanderDecision::Walk]) {
		m_pRunningActivity = m_ActivityMap[AIActivityType::Walk];
		AILog("[배회] 걷기\n");
		return;
	}

	m_pRunningActivity = m_ActivityMap[AIActivityType::Idle];
	AILog("[배회] 휴식\n");
}
void AIComponent::selectTrackActivity() {
	const float fRnd = SGRandom::random_real(0.0f, 100.0f);
	
	if (fRnd < m_pAIInfo->TrackProbs[AITrackDecision::Wander]) {
		AILog("[추격] 배회 시도\n");
		selectWanderActivity();
		return;
	}

	m_eActivityState = AIState::Track;

	if (fRnd < m_pAIInfo->TrackProbs[AITrackDecision::Attack]) {
		AILog("[추격] 공격 시도\n");
		m_pRunningActivity = m_ActivityMap[AIActivityType::Attack];
		return;
	}

	AILog("[추격] 걷기 시도\n");
	m_pRunningActivity = m_ActivityMap[AIActivityType::Walk];
}

void AIComponent::selectAngryActivity() {
	const float fRnd = SGRandom::random_real(0.0f, 100.0f);

	if (fRnd < m_pAIInfo->AngryProbs[AIAngryDecision::Wander]) {
		AILog("[분노] 배회 시도\n");
		selectWanderActivity();
		return;
	}

	if (fRnd < m_pAIInfo->AngryProbs[AIAngryDecision::Track]) {
		AILog("[분노] 추격 시도\n");
		selectTrackActivity();
		return;
	}

	AILog("[분노] 공격 시도\n");
	m_eActivityState = AIState::Angry;
	m_pRunningActivity = m_ActivityMap[AIActivityType::Attack];
}

void AIComponent::runActivity(AIActivity* activity) {
	DebugAssert(activity);

	if (m_pRunningActivity && m_pRunningActivity->isRunning()) {
		m_pRunningActivity->stop();
	}

	m_pRunningActivity = activity;

	if (m_pRunningActivity)
		m_pRunningActivity->run();
}

void AIComponent::runActivity(AIActivityType_t activityType) {
	DebugAssertMsg(activityType >= 0 && activityType < AIActivityType::Max, "액티비티 타입이 올바르지 않습니다.");

	if (m_pRunningActivity && m_pRunningActivity->isRunning()) {
		m_pRunningActivity->stop();
	}

	m_pRunningActivity = m_ActivityMap[activityType];

	if (m_pRunningActivity)
		m_pRunningActivity->run();
}

SGVec2 AIComponent::getRandomSightPos() {
	SGVec2 curPos = m_pActor->getPositionRealCenter();
	float randRad = SGRandom::random_real(0.0f, 2 * SG_PI);
	SGVec2 dstPos{
		curPos.x + m_pAIInfo->SightRadious * cosf(randRad),
		curPos.y + m_pAIInfo->SightRadious * sinf(randRad)
	};

	return curPos.lerp(dstPos, SGRandom::random_real(SightRandomPosMinAlpha, 1.0f));
}

AIActivity* AIComponent::getRunningActivity() const {
	return m_pRunningActivity;
}
