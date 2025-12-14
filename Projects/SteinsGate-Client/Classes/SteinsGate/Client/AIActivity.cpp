/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 4:58:10 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "AIActivity.h"

//////////////////////////////////////////////////////////////////////////////////////////
AIActivity::AIActivity(Actor* _pActor, AIActivityType_t _type)
	: actor_(_pActor)
	, type_(_type)
	, state_(eInitialized)
	, elapsedTime_(0.0f)
	, limitTime_(1.0f)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIActivity::run()
{
	DebugAssertMsg(state_ != eRunning, "해당 AI 액터의 액티비티가 실행중입니다.");
	state_ = eRunning;
	elapsedTime_ = 0.0f;
	onActivityBegin();
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIActivity::stop()
{
	if (!isRunning())
		return;

	state_ = eFinished;
	onActivityEnd();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool AIActivity::isRunning()
{
	return state_ == eRunning;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIActivity::updateLimitTime(float _dt)
{
	if (!isRunning())
		return;

	elapsedTime_ += _dt;

	if (elapsedTime_ >= limitTime_)
	{
		stop();
	}
}
