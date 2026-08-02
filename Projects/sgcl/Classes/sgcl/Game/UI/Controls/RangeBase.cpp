/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/RangeBase.h"

namespace sgui
{

void RangeBase::SetValue(float _value)
{
	const float clamped = ClampF(_value, minimum_, maximum_);

	if (clamped == value_)
	{
		return;
	}

	const float oldValue = value_;
	value_ = clamped;

	OnValueChanged(oldValue, value_);

	UIEventRange event;
	event.type_ = ueValueChanged;
	event.oldValue_ = oldValue;
	event.newValue_ = value_;
	RaiseEvent(&event);
}

float RangeBase::GetRatio() const
{
	const float range = maximum_ - minimum_;

	if (range <= 0.0f)
	{
		return 0.0f;
	}

	return (value_ - minimum_) / range;
}

} // namespace sgui
