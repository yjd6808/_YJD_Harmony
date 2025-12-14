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

	AIActivity(Actor* _pActor, AIActivityType_t _type);
	virtual ~AIActivity() = default;

	virtual void run();
	virtual void stop();

	bool isRunning();
	AIActivityType_t getType() { return type_; }

	void updateLimitTime(float _dt);

	virtual void onUpdate(float _dt) = 0;
	virtual void onActivitySelectFromAIRoutine(AIInfo* _pAiInfo, AIState_t _aiState) {}      // AI 루틴 실행중 선택된 경우
	virtual void onActivityBegin() = 0;
	virtual void onActivityEnd() {}
	virtual void onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) {}
	virtual void onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) {}
	virtual void onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) {}
	virtual void onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame) {}

protected:
	JCORE_NOT_NULL Actor* actor_;

	AIActivityType_t type_;
	State state_;

	float elapsedTime_;
	float limitTime_;
};
