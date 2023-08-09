/*
 * 작성자: 윤정도
 * 생성일: 1/28/2023 7:00:27 PM
 * =====================
 * 내가 정립한 AI 개념이 적용될 엑터
 *  - 몬스터
 *
 */


#pragma once

#include <SteinsGate/Client/Actor.h>
#include <SteinsGate/Client/HitRecorder.h>
#include <SteinsGate/Client/AIActivity.h>
#include <SteinsGate/Client/AIInfo.h>

class AIActor : public Actor
{
public:
	enum State
	{
		eNone,
		eWander,
		eTrack,
		eAngry
	};
public:
	AIActor(ActorType_t type, int code, AIInfo* aiInfo);
	~AIActor() override = 0;

	void initAI();

	virtual void initAIActivities() = 0;
	virtual void onSelectedActivity(AIActivity* selectedActivity) = 0;

	bool initVariables() override;

	// TODO: 분할 정복 방식으로 미리 확률 계산해놓는것 고려.
	// void initAIProbs(); 

	// TODO: AI 중간에 변경 가능할 수 있을 수도 있다. 현재 진행중인 액티비티가 끝난 후 수행해주도록
	// (쓸일 있을까?, 갑자기 몬스터 태세 전환 한다든가 할 때 괜찮을 듯)
	// void exhangeAI(SGAIInfo* exchangeInfo);

	
	void update(float dt) override;
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
	AIInfo* getAiInfo() { return m_pAiInfo; }
	State getState() { return m_eState; }
	Actor* getTarget() { return m_pTarget; }

protected:
	AIActivity* m_ActivityMap[AIActivityType::Max];
	AIActivity* m_pRunningActivity;
	Actor* m_pTarget;
	AIInfo* m_pAiInfo;


	State m_eState;
	State m_eActivityState;	
	State m_ePreviousState;
};

inline AIActor::~AIActor() { /* 일단 생성 불가로 만들기위한 용도 */ }