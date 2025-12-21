/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 10:08:04 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/AIInfo.h>
#include <SteinsGate/Client/AttackActivity.h>
#include <SteinsGate/Client/FallDownActivity.h>
#include <SteinsGate/Client/HitActivity.h>
#include <SteinsGate/Client/IdleActivity.h>
#include <SteinsGate/Client/WalkActivity.h>
#include <SteinsGate/Client/SitActivity.h>
#include <SteinsGate/Client/ActorComponent.h>

class AIComponent : public ActorComponent, public IUpdatable
{
public:
	AIComponent(Actor* _pActor);
	~AIComponent() override;

	void Initialize() override;
	void OnUpdate(float _dt) override;

	void SetAiInfo(AIInfo* _pAIInfo);

	void SetAttackActivity(AttackActivity* _pAttackActivity);
	void SetFallDownActivity(FallDownActivity* _pFallDownActivity);
	void SetHitActivity(HitActivity* _pHitActivity);
	void SetIdleActivity(IdleActivity* _pIdleActivity);
	void SetWalkActivity(WalkActivity* _pWalkActivity);
	void SetSitActivity(SitActivity* _pSitActivity);

	// TODO: 분할 정복 방식으로 미리 확률 계산해놓는것 고려.
	// void initAIProbs(); 

	// TODO: AI 중간에 변경 가능할 수 있을 수도 있다. 현재 진행중인 액티비티가 끝난 후 수행해주도록
	// (쓸일 있을까?, 갑자기 몬스터 태세 전환 한다든가 할 때 괜찮을 듯)
	// void exhangeAI(SGAIInfo* exchangeInfo);

	void UpdateState();
	void UpdateActivity(float _dt);
	void SelectActivity();
	void UpdateDirection();

	void SelectWanderActivity();
	void SelectTrackActivity();
	void SelectAngryActivity();

	void RunActivity(AIActivity* _pActivity);
	void RunActivity(AIActivityType_t _activityType);

	SGVec2				GetRandomSightPos();
	AIInfo*				GetAiInfo() { return pAIInfo_; }
	AIState_t			GetState() { return state_; }
	Actor*				GetTarget() { return pTarget_; }
	AIActivity*			GetRunningActivity() const;

	SG_COMPONENT_TYPE_GETTER(Type::eAI)

private:
	JCORE_NULLABLE AIInfo* pAIInfo_;
	JCORE_NULLABLE Actor* pTarget_;

	JCORE_NULLABLE AIActivity* activityMap_[AIActivityType::Max];
	JCORE_NULLABLE AIActivity* runningActivity_;

	AIState_t state_;
	AIState_t activityState_;
	AIState_t previousState_;
};
