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
//////////////////////////////////////////////////////////////////////////////////////////
{
	lastCalcTime_ = JCore::Env::TimeGetTime();
}

//////////////////////////////////////////////////////////////////////////////////////////
CStackingBuffer::CStackingBuffer(const CStackingBuffer& _other)
{
	this->operator=(_other);
}

//////////////////////////////////////////////////////////////////////////////////////////
CStackingBuffer& CStackingBuffer::operator=(const CStackingBuffer& _other)
{
	if (this == &_other)
		return *this;

	id_ = _other.id_;
	type_ = _other.type_;
	deltaPerSec_ = _other.deltaPerSec_;
	duration_ = _other.duration_;
	percentBase_ = _other.percentBase_;
	isPercentStacking_ = _other.isPercentStacking_;
	isDirty_ = _other.isDirty_;
	lastCalcTime_ = _other.lastCalcTime_;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
float CStackingBuffer::CalcDelta()
{
	Int32U currTime = JCore::Env::TimeGetTime();
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