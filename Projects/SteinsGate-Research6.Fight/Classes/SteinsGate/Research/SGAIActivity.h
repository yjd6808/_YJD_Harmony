/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 4:57:59 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGStruct.h>
#include <SteinsGate/Research/SGHitRecorder.h>

class SGAIActor;
class SGActorPartAnimation;
class SGFrameTexture;
class SGAIActivity
{
public:
	enum State
	{
		eInitialized,
		eRunning,
		eFinished,
	};

	SGAIActivity(AIActivity_t type);
	virtual ~SGAIActivity() = default;
	virtual SGAIActor* getAIActor() = 0;

	virtual void run();
	virtual void stop();

	bool isRunning();
	void setLimit(float limit);
	AIActivity_t getType() { return m_eType; }

	void updateLimitTime(float dt);

	virtual void onUpdate(float dt) = 0;
	virtual void onActivityBegin() = 0;
	virtual void onActivityEnd() {}
	virtual void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* frame);
	virtual void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame);
	virtual void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* frame);
	virtual void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame);
protected:
	AIActivity_t m_eType;
	State m_eState;

	float m_fElasedTime;
	float m_fLimitTime;
};
