/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 4:57:59 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGStruct.h>

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

	SGAIActivity(SGAIActor* actor, AIActivity_t type);
	virtual ~SGAIActivity() = default;

	void run();
	void stop();
	bool isRunning();
	AIActivity_t getType() { return m_eType; }

	virtual void onUpdate(float dt) = 0;
	virtual void onActivityBegin() = 0;
	virtual void onActivityEnd() {}
	virtual void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* frame);
	virtual void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame);
	virtual void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* frame);
	virtual void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame);
protected:
	State m_eState;
	AIActivity_t m_eType;
	SGAIActor* m_pActor;
};
