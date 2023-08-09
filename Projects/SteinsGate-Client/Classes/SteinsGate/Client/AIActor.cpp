/*
 * 작성자: 윤정도
 * 생성일: 1/28/2023 7:00:35 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "AIActor.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/MapLayer.h>
#include <SteinsGate/Client/SGVec2Ex.h>

#define SightRandomPosMinAlpha 0.2f
#define EnableLog false

#if EnableLog
#define AILog(fmt, ...) Log(fmt, ##__VA_ARGS__)
#else
#define AILog(...)
#endif

AIActor::AIActor(ActorType_t type, int code, AIInfo* aiInfo)
	: Actor(type, code)
	, m_pAiInfo(aiInfo)
	, m_ActivityMap{}
	, m_pRunningActivity()
	, m_eState(eNone)
	, m_ePreviousState(eNone)
	, m_pTarget(nullptr)
{}

void AIActor::initAI() {
	initAIActivities();
	//TODO: initAIProbs();
}


bool AIActor::initVariables() {
	Actor::initVariables();

	// m_pAiInfo = aiInfo;
	// m_ActivityMap = ;

	m_pRunningActivity = m_ActivityMap[AIActivityType::Idle];
	m_eState = eWander;
	m_ePreviousState = eNone;
	m_pTarget = nullptr;

	return true;
}

void AIActor::update(float dt) {
	Actor::update(dt);

	DebugAssertMsg(m_pAiInfo != nullptr, "AI 정보가 주입되지 않았습니다.");
	DebugAssertMsg(m_pRunningActivity != nullptr, "동작중인 액티비티가 없습니다.");

	updateState();				// 변경가능한 상태 확인
	selectActivity();			// 해당 상태에서 수행가능한 액티비티 설정
	updateDirection();
	updateActivity(dt);			// 액티비티 지속 업데이트
}

void AIActor::updateState() {
	if (m_pRunningActivity->isRunning()) {
		return;
	}

	m_ePreviousState = m_eState;
	float enemyDist;
	Actor* pPreviousTarget = m_pTarget;
	m_pTarget = m_pMapLayer->findNearestCharacterInRadious(this, m_pAiInfo->SightRadious, enemyDist);

	if (m_pTarget == nullptr) {
		AILog("[상태] 적 미발견: 배회 상태\n");
		m_eState = eWander;
		return;
	}

	// TODO: 타겟 변경 알려줘야할까?
	if (pPreviousTarget == nullptr) {
	}

	if (enemyDist > m_pAiInfo->AttackRadious) {
		AILog("[상태] 적 발견: 추격 상태\n");
		m_eState = eTrack;
		return;
	}

	AILog("[상태] 적 근접: 분노 상태\n");
	m_eState = eAngry;
}



void AIActor::selectActivity() {
	if (m_pRunningActivity->isRunning())
		return;

	switch (m_eState) {
	case eWander:	selectWanderActivity();		break;
	case eTrack:	selectTrackActivity();		break;
	case eAngry:	selectAngryActivity();		break;
	default: DebugAssertMsg(false, "이상한 AI 상태입니다.");
	}

	onSelectedActivity(m_pRunningActivity);
	m_pRunningActivity->run();
}

void AIActor::updateActivity(float dt) {
	m_pRunningActivity->onUpdate(dt);
	
}

void AIActor::updateDirection() {
	AIActivityType_t eActivityType = m_pRunningActivity->getType();

	if (eActivityType != AIActivityType::Idle && 
		eActivityType != AIActivityType::Walk) {
		return;
	}

	if (m_eState >= eTrack && m_pTarget) {
		SpriteDirection_t eWhereIsTarget;
		SGVec2Ex::getLookDirectionX(getPositionRealCenterX(), m_pTarget->getPositionRealCenterX(), eWhereIsTarget);
		setSpriteDirection(eWhereIsTarget);
	}
}

void AIActor::selectWanderActivity() {
	float fResult = SGRandom::random_real(0.0f, 100.0f);
	m_eActivityState = State::eWander;

	if (fResult < m_pAiInfo->WanderProbs[AIWanderDecision::Walk]) {
		m_pRunningActivity = m_ActivityMap[AIActivityType::Walk];
		AILog("[배회] 걷기\n");
		return;
	}

	m_pRunningActivity = m_ActivityMap[AIActivityType::Idle];
	AILog("[배회] 휴식\n");
}
void AIActor::selectTrackActivity() {
	float fResult = SGRandom::random_real(0.0f, 100.0f);
	
	if (fResult < m_pAiInfo->TrackProbs[AITrackDecision::Wander]) {
		AILog("[추격] 배회 시도\n");
		selectWanderActivity();
		return;
	}

	m_eActivityState = State::eTrack;

	if (fResult < m_pAiInfo->TrackProbs[AITrackDecision::Attack]) {
		AILog("[추격] 공격 시도\n");
		m_pRunningActivity = m_ActivityMap[AIActivityType::Attack];
		return;
	}

	AILog("[추격] 걷기 시도\n");
	m_pRunningActivity = m_ActivityMap[AIActivityType::Walk];
}

void AIActor::selectAngryActivity() {
	float fResult = SGRandom::random_real(0.0f, 100.0f);

	if (fResult < m_pAiInfo->AngryProbs[AIAngryDecision::Wander]) {
		AILog("[분노] 배회 시도\n");
		selectWanderActivity();
		return;
	}

	if (fResult < m_pAiInfo->AngryProbs[AIAngryDecision::Track]) {
		AILog("[분노] 추격 시도\n");
		selectTrackActivity();
		return;
	}

	AILog("[분노] 공격 시도\n");
	m_eActivityState = State::eAngry;
	m_pRunningActivity = m_ActivityMap[AIActivityType::Attack];
}

void AIActor::runActivity(AIActivity* activity) {
	if (m_pRunningActivity && m_pRunningActivity->isRunning()) {
		m_pRunningActivity->stop();
	}

	m_pRunningActivity = activity;
	m_pRunningActivity->run();
}

void AIActor::runActivity(AIActivityType_t activityType) {
	DebugAssertMsg(activityType >= 0 && activityType < AIActivityType::Max, "액티비티 타입이 올바르지 않습니다.");

	if (m_pRunningActivity && m_pRunningActivity->isRunning()) {
		m_pRunningActivity->stop();
	}

	m_pRunningActivity = m_ActivityMap[activityType];
	m_pRunningActivity->run();
}

SGVec2 AIActor::getRandomSightPos() {
	SGVec2 curPos = getPositionRealCenter();
	float randRad = SGRandom::random_real(0.0f, 2 * SG_PI);
	SGVec2 dstPos{
		curPos.x + m_pAiInfo->SightRadious * cosf(randRad),
		curPos.y + m_pAiInfo->SightRadious * sinf(randRad)
	};

	return curPos.lerp(dstPos, SGRandom::random_real(SightRandomPosMinAlpha, 1.0f));
}
