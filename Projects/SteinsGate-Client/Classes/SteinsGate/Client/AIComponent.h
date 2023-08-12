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
	AIComponent(Actor* actor);
	~AIComponent() override;

	void initialize() override;
	void onUpdate(float dt) override;

	void setAIInfo(AIInfo* info);

	void setAttackActivity(AttackActivity* attackActivity);
	void setFallDownActivity(FallDownActivity* fallDownActivity);
	void setHitActivity(HitActivity* hitActivity);
	void setIdleActivity(IdleActivity* idleActivity);
	void setWalkActivity(WalkActivity* walkActivity);
	void setSitActivity(SitActivity* sitActivity);

	// TODO: 분할 정복 방식으로 미리 확률 계산해놓는것 고려.
	// void initAIProbs(); 

	// TODO: AI 중간에 변경 가능할 수 있을 수도 있다. 현재 진행중인 액티비티가 끝난 후 수행해주도록
	// (쓸일 있을까?, 갑자기 몬스터 태세 전환 한다든가 할 때 괜찮을 듯)
	// void exhangeAI(SGAIInfo* exchangeInfo);

	void updateState();
	void updateActivity(float dt);
	void selectActivity();
	void updateDirection();

	void selectWanderActivity();
	void selectTrackActivity();
	void selectAngryActivity();

	void runActivity(AIActivity* activity);
	void runActivity(AIActivityType_t activityType);

	SGVec2 getRandomSightPos();
	AIInfo* getAiInfo() { return m_pAIInfo; }
	AIState_t getState() { return m_eState; }
	Actor* getTarget() { return m_pTarget; }
	AIActivity* getRunningActivity() const;

	SG_COMPONENT_TYPE_GETTER(Type::eAI)
private:
	JCORE_NULLABLE AIInfo* m_pAIInfo;
	JCORE_NULLABLE Actor* m_pTarget;

	JCORE_NULLABLE AIActivity* m_ActivityMap[AIActivityType::Max];
	JCORE_NULLABLE AIActivity* m_pRunningActivity;
	
	AIState_t m_eState;
	AIState_t m_eActivityState;
	AIState_t m_ePreviousState;
};
