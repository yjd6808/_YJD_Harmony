/*
 * 작성자: 윤정도
 * =====================
 * sgui - 값 범위 컨트롤 베이스 (WPF RangeBase)
 * 값 변경 시 ueValueChanged(UIEventRange) 버블링 이벤트를 발생시킨다.
 */

#pragma once

#include "sgcl/Game/UI/Controls/Control.h"

namespace sgui
{

class RangeBase : public Control
{
public:
	void SetMinimum(float _minimum) { minimum_ = _minimum; SetValue(value_); }
	float GetMinimum() const { return minimum_; }

	void SetMaximum(float _maximum) { maximum_ = _maximum; SetValue(value_); }
	float GetMaximum() const { return maximum_; }

	void SetValue(float _value);
	float GetValue() const { return value_; }

	// 0.0 ~ 1.0 비율
	float GetRatio() const;

protected:
	virtual void OnValueChanged(float _oldValue, float _newValue) { UNUSED(_oldValue); UNUSED(_newValue); }

	float minimum_ = 0.0f;
	float maximum_ = 100.0f;
	float value_ = 0.0f;
};

} // namespace sgui
