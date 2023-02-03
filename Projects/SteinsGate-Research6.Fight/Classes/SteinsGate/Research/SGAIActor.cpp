/*
 * 작성자: 윤정도
 * 생성일: 1/28/2023 7:00:35 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGAIActor.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGMapLayer.h>
#include <SteinsGate/Research/SGVec2Ex.h>

#define SightRandomPosMinAlpha 0.2f
#define EnableLog false

#if EnableLog
#define AILog(fmt, ...) Log(fmt, ##__VA_ARGS__)
#else
#define AILog(...)
#endif

SGAIActor::SGAIActor(ActorType_t type, int code, SGAIInfo* aiInfo)
	: SGPhysicsActor(type, code)
	, m_pAiInfo(aiInfo)
	, m_ActivityMap{}
	, m_pRunningActivity()
	, m_eState(eNone)
	, m_ePreviousState(eNone)
	, m_pTarget(nullptr)
{}

void SGAIActor::initAI() {
	initAIActivities();
	//TODO: initAIProbs();
}


bool SGAIActor::initVariables() {
	SGPhysicsActor::initVariables();

	// m_pAiInfo = aiInfo;
	// m_ActivityMap = ;

	m_pRunningActivity = m_ActivityMap[AIActivity::Idle];
	m_eState = eWander;
	m_ePreviousState = eNone;
	m_pTarget = nullptr;

	return true;
}

void SGAIActor::update(float dt) {
	SGPhysicsActor::update(dt);

	DebugAssertMessage(m_pAiInfo != nullptr, "AI 정보가 주입되지 않았습니다.");
	DebugAssertMessage(m_pRunningActivity != nullptr, "동작중인 액티비티가 없습니다.");

	updateState();				// 변경가능한 상태 확인
	selectActivity();			// 해당 상태에서 수행가능한 액티비티 설정
	updateDirection();
	updateActivity(dt);			// 액티비티 지속 업데이트
}

void SGAIActor::updateState() {
	if (m_pRunningActivity->isRunning()) {
		return;
	}

	m_ePreviousState = m_eState;
	float enemyDist;
	SGActor* pPreviousTarget = m_pTarget;
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



void SGAIActor::selectActivity() {
	if (m_pRunningActivity->isRunning())
		return;

	switch (m_eState) {
	case eWander:	selectWanderActivity();		break;
	case eTrack:	selectTrackActivity();		break;
	case eAngry:	selectAngryActivity();		break;
	default: DebugAssertMessage(false, "이상한 AI 상태입니다.");
	}

	onSelectedActivity(m_pRunningActivity);
	m_pRunningActivity->run();
}

void SGAIActor::updateActivity(float dt) {
	m_pRunningActivity->onUpdate(dt);
	
}

void SGAIActor::updateDirection() {
	AIActivity_t eActivityType = m_pRunningActivity->getType();

	if (eActivityType != AIActivity::Idle && 
		eActivityType != AIActivity::Walk) {
		return;
	}

	if (m_eState >= eTrack && m_pTarget) {
		SpriteDirection_t eWhereIsTarget;
		SGVec2Ex::getLookDirectionX(getPositionRealCenterX(), m_pTarget->getPositionRealCenterX(), eWhereIsTarget);
		setSpriteDirection(eWhereIsTarget);
	}
}

void SGAIActor::selectWanderActivity() {
	float fResult = SGRandom::random_real(0.0f, 100.0f);
	m_eActivityState = State::eWander;

	if (fResult < m_pAiInfo->WanderProbs[AIWanderDecision::Walk]) {
		m_pRunningActivity = m_ActivityMap[AIActivity::Walk];
		AILog("[배회] 걷기\n");
		return;
	}

	m_pRunningActivity = m_ActivityMap[AIActivity::Idle];
	AILog("[배회] 휴식\n");
}
void SGAIActor::selectTrackActivity() {
	float fResult = SGRandom::random_real(0.0f, 100.0f);
	
	if (fResult < m_pAiInfo->TrackProbs[AITrackDecision::Wander]) {
		AILog("[추격] 배회 시도\n");
		selectWanderActivity();
		return;
	}

	m_eActivityState = State::eTrack;

	if (fResult < m_pAiInfo->TrackProbs[AITrackDecision::Attack]) {
		AILog("[추격] 공격 시도\n");
		m_pRunningActivity = m_ActivityMap[AIActivity::Attack];
		return;
	}

	AILog("[추격] 걷기 시도\n");
	m_pRunningActivity = m_ActivityMap[AIActivity::Walk];
}

void SGAIActor::selectAngryActivity() {
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
	m_pRunningActivity = m_ActivityMap[AIActivity::Attack];
}

void SGAIActor::runActivity(SGAIActivity* activity) {
	if (m_pRunningActivity && m_pRunningActivity->isRunning()) {
		m_pRunningActivity->stop();
	}

	m_pRunningActivity = activity;
	m_pRunningActivity->run();
}

void SGAIActor::runActivity(AIActivity_t activityType) {
	DebugAssertMessage(activityType >= 0 && activityType < AIActivity::Max, "액티비티 타입이 올바르지 않습니다.");

	if (m_pRunningActivity && m_pRunningActivity->isRunning()) {
		m_pRunningActivity->stop();
	}

	m_pRunningActivity = m_ActivityMap[activityType];
	m_pRunningActivity->run();
}

SGVec2 SGAIActor::getRandomSightPos() {
	SGVec2 curPos = getPositionRealCenter();
	float randRad = SGRandom::random_real(0.0f, 2 * SG_PI);
	SGVec2 dstPos{
		curPos.x + m_pAiInfo->SightRadious * cosf(randRad),
		curPos.y + m_pAiInfo->SightRadious * sinf(randRad)
	};

	return curPos.lerp(dstPos, SGRandom::random_real(SightRandomPosMinAlpha, 1.0f));
}
