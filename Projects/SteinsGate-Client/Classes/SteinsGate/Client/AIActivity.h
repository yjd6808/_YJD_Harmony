/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 4:57:59 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Declspec.h>

#include <SteinsGate/Client/Struct.h>
#include <SteinsGate/Client/HitRecorder.h>

class ActorPartAnimation;
class FrameTexture;
struct AIInfo;

class JCORE_NOVTABLE AIActivity
{
public:
	enum State
	{
		eInitialized,
		eRunning,
		eFinished,
	};

	AIActivity(Actor* actor, AIActivityType_t type);
	virtual ~AIActivity() = default;

	virtual void run();
	virtual void stop();

	bool isRunning();
	AIActivityType_t getType() { return m_eType; }

	void updateLimitTime(float dt);

	virtual void onUpdate(float dt) = 0;
	virtual void onActivitySelectFromAIRoutine(AIInfo* aiInfo, AIState_t aiState) {}		// AI 루틴 실행중 선택된 경우
	virtual void onActivityBegin() = 0;
	virtual void onActivityEnd() {}
	virtual void onFrameBegin(ActorPartAnimation* animation, FrameTexture* frame) {}
	virtual void onFrameEnd(ActorPartAnimation* animation, FrameTexture* frame) {}
	virtual void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* frame) {}
	virtual void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* frame) {}
protected:
	JCORE_NOT_NULL Actor* m_pActor;

	AIActivityType_t m_eType;
	State m_eState;

	float m_fElasedTime;
	float m_fLimitTime;
};
