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

	void initialize() override;
	void onUpdate(float _dt) override;

	void setAIInfo(AIInfo* _pAIInfo);

	void setAttackActivity(AttackActivity* _pAttackActivity);
	void setFallDownActivity(FallDownActivity* _pFallDownActivity);
	void setHitActivity(HitActivity* _pHitActivity);
	void setIdleActivity(IdleActivity* _pIdleActivity);
	void setWalkActivity(WalkActivity* _pWalkActivity);
	void setSitActivity(SitActivity* _pSitActivity);

	// TODO: 분할 정복 방식으로 미리 확률 계산해놓는것 고려.
	// void initAIProbs(); 

	// TODO: AI 중간에 변경 가능할 수 있을 수도 있다. 현재 진행중인 액티비티가 끝난 후 수행해주도록
	// (쓸일 있을까?, 갑자기 몬스터 태세 전환 한다든가 할 때 괜찮을 듯)
	// void exhangeAI(SGAIInfo* exchangeInfo);

	void updateState();
	void updateActivity(float _dt);
	void selectActivity();
	void updateDirection();

	void selectWanderActivity();
	void selectTrackActivity();
	void selectAngryActivity();

	void runActivity(AIActivity* _pActivity);
	void runActivity(AIActivityType_t _activityType);

	SGVec2 getRandomSightPos();
	AIInfo* getAiInfo() { return aiInfo_; }
	AIState_t getState() { return state_; }
	Actor* getTarget() { return target_; }
	AIActivity* getRunningActivity() const;

	SG_COMPONENT_TYPE_GETTER(Type::eAI)

private:
	JCORE_NULLABLE AIInfo* aiInfo_;
	JCORE_NULLABLE Actor* target_;

	JCORE_NULLABLE AIActivity* activityMap_[AIActivityType::Max];
	JCORE_NULLABLE AIActivity* runningActivity_;

	AIState_t state_;
	AIState_t activityState_;
	AIState_t previousState_;
};
