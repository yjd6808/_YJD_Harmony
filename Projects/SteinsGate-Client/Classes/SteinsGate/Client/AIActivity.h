/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 4:57:59 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Struct.h>
#include <SteinsGate/Client/HitRecorder.h>

class AIActor;
class ActorPartAnimation;
class FrameTexture;
class AIActivity
{
public:
	enum State
	{
		eInitialized,
		eRunning,
		eFinished,
	};

	AIActivity(AIActivityType_t type);
	virtual ~AIActivity() = default;
	virtual AIActor* getAIActor() = 0;

	virtual void run();
	virtual void stop();

	bool isRunning();
	void setLimit(float limit);
	AIActivityType_t getType() { return m_eType; }

	void updateLimitTime(float dt);

	virtual void onUpdate(float dt) = 0;
	virtual void onActivityBegin() = 0;
	virtual void onActivityEnd() {}
	virtual void onFrameBegin(ActorPartAnimation* animation, FrameTexture* frame);
	virtual void onFrameEnd(ActorPartAnimation* animation, FrameTexture* frame);
	virtual void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* frame);
	virtual void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* frame);
protected:
	AIActivityType_t m_eType;
	State m_eState;

	float m_fElasedTime;
	float m_fLimitTime;
};
