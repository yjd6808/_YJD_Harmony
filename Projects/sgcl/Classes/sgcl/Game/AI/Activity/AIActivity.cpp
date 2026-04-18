/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 4:58:10 AM
 * =====================
 *
 */

#include "Game/AI/Activity/AIActivity.h"

//////////////////////////////////////////////////////////////////////////////////////////
AIActivity::AIActivity(Actor* _pActor, AIActivityType_t _type)
: pActor_(_pActor)
, type_(_type)
, state_(eInitialized)
, elapsedTime_(0.0f)
, limitTime_(1.0f)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIActivity::Run()
{
	jc_assert_msg(state_ != eRunning, "해당 AI 액터의 액티비티가 실행중입니다.");
	state_ = eRunning;
	elapsedTime_ = 0.0f;
	OnActivityBegin();
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIActivity::Stop()
{
	if (!IsRunning())
		return;

	state_ = eFinished;
	OnActivityEnd();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool AIActivity::IsRunning()
{
	return state_ == eRunning;
}

//////////////////////////////////////////////////////////////////////////////////////////
void AIActivity::UpdateLimitTime(float _dt)
{
	if (!IsRunning())
		return;

	elapsedTime_ += _dt;

	if (elapsedTime_ >= limitTime_)
	{
		Stop();
	}
}
