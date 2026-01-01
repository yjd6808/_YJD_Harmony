/*
 * 작성자: 윤정도
 * 생성일: 12/1/2025 12:26:50 PM
 * =====================
 *
 */

#include "Core.h"
#include "StackingBuffer.h"

//////////////////////////////////////////////////////////////////////////////////////////
CStackingBuffer::CStackingBuffer(
	Int32 _id,
	float _deltaPerSec,
	Int32U _duration,
	Int32U _type,
	bool _isPercentStacking,
	float _percentBase)
{
	lastCalcTime_ = jc::Env::TimeGetTime();
}

//////////////////////////////////////////////////////////////////////////////////////////
float CStackingBuffer::CalcDelta()
{
	Int32U currTime = jc::Env::TimeGetTime();
	Int32U elapsedTime = currTime - lastCalcTime_;

	if (elapsedTime > duration_)
		elapsedTime = duration_;

	float delta = 0.f;
	if (isPercentStacking_)
	{
		delta = (deltaPerSec_ * percentBase_ * elapsedTime) / (1000.f * 100);
	}
	else
	{
		delta = (deltaPerSec_ * elapsedTime) / 1000.f;
	}

	// variable set
	duration_ -= elapsedTime;
	lastCalcTime_ = currTime;
	return delta;
}
