/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 10:08:14 AM
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "AIComponent.h"

#include <sgcl/_Layer/Layer_Map.h>
#include <sgcl/SGVec2Ex.h>

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
, pAIInfo_(nullptr)
, pTarget_(nullptr)
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
void AIComponent::Initialize()
{
	runningActivity_ = nullptr;
	state_ = AIState::Wander;
	previousState_ = AIState::None;
	pTarget_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::OnUpdate(float _dt)
{
	if (pAIInfo_ == nullptr)
		return;

	UpdateState(); // 변경가능한 상태 확인
	SelectActivity(); // 해당 상태에서 수행가능한 액티비티 설정
	UpdateDirection();
	UpdateActivity(_dt); // 액티비티 지속 업데이트
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::SetAiInfo(AIInfo* _pAIInfo)
{
	pAIInfo_ = _pAIInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::SetAttackActivity(AttackActivity* _pAttackActivity)
{
	jc_assert(activityMap_[AIActivityType::Attack] == nullptr);
	activityMap_[AIActivityType::Attack] = _pAttackActivity;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::SetFallDownActivity(FallDownActivity* _pFallDownActivity)
{
	jc_assert(activityMap_[AIActivityType::FallDown] == nullptr);
	activityMap_[AIActivityType::FallDown] = _pFallDownActivity;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::SetHitActivity(HitActivity* _pHitActivity)
{
	jc_assert(activityMap_[AIActivityType::Hit] == nullptr);
	activityMap_[AIActivityType::Hit] = _pHitActivity;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::SetIdleActivity(IdleActivity* _pIdleActivity)
{
	jc_assert(activityMap_[AIActivityType::Idle] == nullptr);
	activityMap_[AIActivityType::Idle] = _pIdleActivity;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::SetWalkActivity(WalkActivity* _pWalkActivity)
{
	jc_assert(activityMap_[AIActivityType::Walk] == nullptr);
	activityMap_[AIActivityType::Walk] = _pWalkActivity;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::SetSitActivity(SitActivity* _pSitActivity)
{
	jc_assert(activityMap_[AIActivityType::Sit] == nullptr);
	activityMap_[AIActivityType::Sit] = _pSitActivity;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::UpdateState()
{
	MapLayer* pMapLayer = pActor_->GetMapLayer();

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
	Actor* pPreviousTarget = pTarget_;
	pTarget_ = pMapLayer->FindNearestCharacterInRadious(pActor_, pAIInfo_->sightRadious_, enemyDist);

	if (pTarget_ == nullptr)
	{
		AILog("[상태] 적 미발견: 배회 상태\n");
		state_ = AIState::Wander;
		return;
	}

	// TODO: 타겟 변경 알려줘야할까?
	if (pPreviousTarget == nullptr)
	{
	}

	if (enemyDist > pAIInfo_->attackRadious_)
	{
		AILog("[상태] 적 발견: 추격 상태\n");
		state_ = AIState::Track;
		return;
	}

	AILog("[상태] 적 근접: 분노 상태\n");
	state_ = AIState::Angry;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::SelectActivity()
{
	if (runningActivity_ && runningActivity_->IsRunning())
		return;

	switch (state_)
	{
	case AIState::Wander: SelectWanderActivity();
		break;
	case AIState::Track: SelectTrackActivity();
		break;
	case AIState::Angry: SelectAngryActivity();
		break;
	}

	if (runningActivity_ == nullptr)
		return;

	runningActivity_->OnActivitySelectFromAiRoutine(pAIInfo_, activityState_);
	runningActivity_->Run();
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::UpdateActivity(float _dt)
{
	if (runningActivity_ == nullptr)
		return;

	runningActivity_->OnUpdate(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::UpdateDirection()
{
	if (runningActivity_ == nullptr)
		return;

	AIActivityType_t activityType = runningActivity_->GetType();

	if (activityType != AIActivityType::Idle &&
		activityType != AIActivityType::Walk)
	{
		return;
	}

	if (state_ >= AIState::Track && pTarget_)
	{
		SpriteDirection_t whereIsTarget;
		SGVec2Ex::GetLookDirectionX(pActor_->GetPositionRealCenterX(), pTarget_->GetPositionRealCenterX(),
		                            whereIsTarget);
		pActor_->SetSpriteDirection(whereIsTarget);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::SelectWanderActivity()
{
	const float rnd = Random::GenerateF(0.0f, 100.0f);
	activityState_ = AIState::Wander;

	if (rnd < pAIInfo_->wanderProbs_[AIWanderDecision::Walk])
	{
		runningActivity_ = activityMap_[AIActivityType::Walk];
		AILog("[배회] 걷기\n");
		return;
	}

	runningActivity_ = activityMap_[AIActivityType::Idle];
	AILog("[배회] 휴식\n");
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::SelectTrackActivity()
{
	const float rnd = jc::Random::GenerateF(0.0f, 100.0f);

	if (rnd < pAIInfo_->trackProbs_[AITrackDecision::Wander])
	{
		AILog("[추격] 배회 시도\n");
		SelectWanderActivity();
		return;
	}

	activityState_ = AIState::Track;

	if (rnd < pAIInfo_->trackProbs_[AITrackDecision::Attack])
	{
		AILog("[추격] 공격 시도\n");
		runningActivity_ = activityMap_[AIActivityType::Attack];
		return;
	}

	AILog("[추격] 걷기 시도\n");
	runningActivity_ = activityMap_[AIActivityType::Walk];
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::SelectAngryActivity()
{
	const float rnd = jc::Random::GenerateF(0.0f, 100.0f);

	if (rnd < pAIInfo_->angryProbs_[AIAngryDecision::Wander])
	{
		AILog("[분노] 배회 시도\n");
		SelectWanderActivity();
		return;
	}

	if (rnd < pAIInfo_->angryProbs_[AIAngryDecision::Track])
	{
		AILog("[분노] 추격 시도\n");
		SelectTrackActivity();
		return;
	}

	AILog("[분노] 공격 시도\n");
	activityState_ = AIState::Angry;
	runningActivity_ = activityMap_[AIActivityType::Attack];
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::RunActivity(AIActivity* _pActivity)
{
	jc_assert(_pActivity);

	if (runningActivity_ && runningActivity_->IsRunning())
	{
		runningActivity_->Stop();
	}

	runningActivity_ = _pActivity;

	if (runningActivity_)
		runningActivity_->Run();
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIComponent::RunActivity(AIActivityType_t _activityType)
{
	jc_assert_msg(_activityType >= 0 && _activityType < AIActivityType::Max, "액티비티 타입이 올바르지 않습니다.");

	if (runningActivity_ && runningActivity_->IsRunning())
	{
		runningActivity_->Stop();
	}

	runningActivity_ = activityMap_[_activityType];

	if (runningActivity_)
		runningActivity_->Run();
}

//////////////////////////////////////////////////////////////////////////////////////////
cc::vec2 AIComponent::GetRandomSightPos()
{
	cc::vec2 curPos = pActor_->GetPositionRealCenter();
	float randRad = jc::Random::GenerateF(0.0f, 2 * SG_PI);
	cc::vec2 dstPos{
		curPos.x + pAIInfo_->sightRadious_ * cosf(randRad),
		curPos.y + pAIInfo_->sightRadious_ * sinf(randRad)
	};

	return curPos.lerp(dstPos, jc::Random::GenerateF(SightRandomPosMinAlpha, 1.0f));
}

//////////////////////////////////////////////////////////////////////////////////////////
AIActivity* AIComponent::GetRunningActivity() const
{
	return runningActivity_;
}
