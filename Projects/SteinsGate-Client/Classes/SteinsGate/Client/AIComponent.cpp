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

//////////////////////////////////////////////////////////////////////////////////////////
AIComponent::AIComponent(Actor* _pActor)
: ActorComponent(_pActor)
, aiInfo_(nullptr)
, target_(nullptr)
, activityMap_{}
, runningActivity_(nullptr)
, state_(AIState::None)
, activityState_(AIState::None)
, previousState_(AIState::None)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
AIComponent::~AIComponent()
{
	Arrays::ForEachDelete(activityMap_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::initialize()
{
	runningActivity_ = nullptr;
	state_ = AIState::Wander;
	previousState_ = AIState::None;
	target_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::onUpdate(float _dt)
{
	if (aiInfo_ == nullptr)
		return;

	updateState(); // 변경가능한 상태 확인
	selectActivity(); // 해당 상태에서 수행가능한 액티비티 설정
	updateDirection();
	updateActivity(_dt); // 액티비티 지속 업데이트
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::setAIInfo(AIInfo* _pAIInfo)
{
	aiInfo_ = _pAIInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::setAttackActivity(AttackActivity* _pAttackActivity)
{
	DebugAssert(activityMap_[AIActivityType::Attack] == nullptr);
	activityMap_[AIActivityType::Attack] = _pAttackActivity;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::setFallDownActivity(FallDownActivity* _pFallDownActivity)
{
	DebugAssert(activityMap_[AIActivityType::FallDown] == nullptr);
	activityMap_[AIActivityType::FallDown] = _pFallDownActivity;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::setHitActivity(HitActivity* _pHitActivity)
{
	DebugAssert(activityMap_[AIActivityType::Hit] == nullptr);
	activityMap_[AIActivityType::Hit] = _pHitActivity;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::setIdleActivity(IdleActivity* _pIdleActivity)
{
	DebugAssert(activityMap_[AIActivityType::Idle] == nullptr);
	activityMap_[AIActivityType::Idle] = _pIdleActivity;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::setWalkActivity(WalkActivity* _pWalkActivity)
{
	DebugAssert(activityMap_[AIActivityType::Walk] == nullptr);
	activityMap_[AIActivityType::Walk] = _pWalkActivity;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::setSitActivity(SitActivity* _pSitActivity)
{
	DebugAssert(activityMap_[AIActivityType::Sit] == nullptr);
	activityMap_[AIActivityType::Sit] = _pSitActivity;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::updateState()
{
	MapLayer* pMapLayer = pActor_->getMapLayer();

	if (pMapLayer == nullptr)
	{
		return;
	}

	if (runningActivity_ && runningActivity_->IsRunning())
	{
		return;
	}

	previousState_ = state_;
	float enemyDist;
	Actor* pPreviousTarget = target_;
	target_ = pMapLayer->findNearestCharacterInRadious(pActor_, aiInfo_->sightRadious_, enemyDist);

	if (target_ == nullptr)
	{
		AILog("[상태] 적 미발견: 배회 상태\n");
		state_ = AIState::Wander;
		return;
	}

	// TODO: 타겟 변경 알려줘야할까?
	if (pPreviousTarget == nullptr)
	{
	}

	if (enemyDist > aiInfo_->attackRadious_)
	{
		AILog("[상태] 적 발견: 추격 상태\n");
		state_ = AIState::Track;
		return;
	}

	AILog("[상태] 적 근접: 분노 상태\n");
	state_ = AIState::Angry;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::selectActivity()
{
	if (runningActivity_ && runningActivity_->IsRunning())
		return;

	switch (state_)
	{
	case AIState::Wander: selectWanderActivity();
		break;
	case AIState::Track: selectTrackActivity();
		break;
	case AIState::Angry: selectAngryActivity();
		break;
	}

	if (runningActivity_ == nullptr)
		return;

	runningActivity_->OnActivitySelectFromAiRoutine(aiInfo_, activityState_);
	runningActivity_->Run();
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::updateActivity(float _dt)
{
	if (runningActivity_ == nullptr)
		return;

	runningActivity_->OnUpdate(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::updateDirection()
{
	if (runningActivity_ == nullptr)
		return;

	AIActivityType_t activityType = runningActivity_->GetType();

	if (activityType != AIActivityType::Idle &&
		activityType != AIActivityType::Walk)
	{
		return;
	}

	if (state_ >= AIState::Track && target_)
	{
		SpriteDirection_t whereIsTarget;
		SGVec2Ex::getLookDirectionX(pActor_->getPositionRealCenterX(), target_->getPositionRealCenterX(),
		                            whereIsTarget);
		pActor_->setSpriteDirection(whereIsTarget);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::selectWanderActivity()
{
	const float rnd = SGRandom::random_real(0.0f, 100.0f);
	activityState_ = AIState::Wander;

	if (rnd < aiInfo_->wanderProbs_[AIWanderDecision::Walk])
	{
		runningActivity_ = activityMap_[AIActivityType::Walk];
		AILog("[배회] 걷기\n");
		return;
	}

	runningActivity_ = activityMap_[AIActivityType::Idle];
	AILog("[배회] 휴식\n");
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::selectTrackActivity()
{
	const float rnd = SGRandom::random_real(0.0f, 100.0f);

	if (rnd < aiInfo_->trackProbs_[AITrackDecision::Wander])
	{
		AILog("[추격] 배회 시도\n");
		selectWanderActivity();
		return;
	}

	activityState_ = AIState::Track;

	if (rnd < aiInfo_->trackProbs_[AITrackDecision::Attack])
	{
		AILog("[추격] 공격 시도\n");
		runningActivity_ = activityMap_[AIActivityType::Attack];
		return;
	}

	AILog("[추격] 걷기 시도\n");
	runningActivity_ = activityMap_[AIActivityType::Walk];
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::selectAngryActivity()
{
	const float rnd = SGRandom::random_real(0.0f, 100.0f);

	if (rnd < aiInfo_->angryProbs_[AIAngryDecision::Wander])
	{
		AILog("[분노] 배회 시도\n");
		selectWanderActivity();
		return;
	}

	if (rnd < aiInfo_->angryProbs_[AIAngryDecision::Track])
	{
		AILog("[분노] 추격 시도\n");
		selectTrackActivity();
		return;
	}

	AILog("[분노] 공격 시도\n");
	activityState_ = AIState::Angry;
	runningActivity_ = activityMap_[AIActivityType::Attack];
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::runActivity(AIActivity* _pActivity)
{
	DebugAssert(_pActivity);

	if (runningActivity_ && runningActivity_->IsRunning())
	{
		runningActivity_->Stop();
	}

	runningActivity_ = _pActivity;

	if (runningActivity_)
		runningActivity_->Run();
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::runActivity(AIActivityType_t _activityType)
{
	DebugAssertMsg(_activityType >= 0 && _activityType < AIActivityType::Max, "액티비티 타입이 올바르지 않습니다.");

	if (runningActivity_ && runningActivity_->IsRunning())
	{
		runningActivity_->Stop();
	}

	runningActivity_ = activityMap_[_activityType];

	if (runningActivity_)
		runningActivity_->Run();
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 AIComponent::getRandomSightPos()
{
	SGVec2 curPos = pActor_->getPositionRealCenter();
	float randRad = SGRandom::random_real(0.0f, 2 * SG_PI);
	SGVec2 dstPos{
		curPos.x + aiInfo_->sightRadious_ * cosf(randRad),
		curPos.y + aiInfo_->sightRadious_ * sinf(randRad)
	};

	return curPos.lerp(dstPos, SGRandom::random_real(SightRandomPosMinAlpha, 1.0f));
}

//////////////////////////////////////////////////////////////////////////////////////////
AIActivity* AIComponent::getRunningActivity() const
{
	return runningActivity_;
}
