/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 4:57:59 AM
 * =====================
 *
 */

#pragma once

#include <jc/Declspec.h>

#include <sgcl/Struct.h>
#include <sgcl/HitRecorder.h>

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

	virtual void Run();
	virtual void Stop();

	bool IsRunning();
	AIActivityType_t GetType() { return type_; }

	void UpdateLimitTime(float _dt);

	virtual void OnUpdate(float _dt) = 0;

	virtual void OnActivitySelectFromAiRoutine(AIInfo* _pAiInfo, AIState_t _aiState)
	{
	} // AI 루틴 실행중 선택된 경우
	virtual void OnActivityBegin() = 0;

	virtual void OnActivityEnd()
	{
	}

	virtual void OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
	{
	}

	virtual void OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
	{
	}

	virtual void OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
	{
	}

	virtual void OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
	{
	}

protected:
	NOT_NULL Actor* pActor_;
	AIActivityType_t type_;
	State state_;
	float elapsedTime_;
	float limitTime_;
};
