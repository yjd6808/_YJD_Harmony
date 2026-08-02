/*
 * 작성자: 윤정도
 * =====================
 * sgui - 버튼 (WPF ButtonBase / Button)
 * Content 속성으로 임의의 UI 요소를 내부에 담을 수 있다.
 * 클릭 시 ueClick 버블링 이벤트를 발생시킨다.
 */

#pragma once

#include "sgcl/Game/UI/Controls/ContentControl.h"

namespace sgui
{

class ButtonBase : public ContentControl
{
public:
	using ClickCallback = std::function<void(ButtonBase*)>;

	ButtonBase();

	void SetClickCallback(const ClickCallback& _callback) { clickCallback_ = _callback; }

	void SimulateClick() { OnClick(); }

	virtual void OnEventRaised(int _id, UIEvent* _pEvent) override;

protected:
	virtual void OnClick();

	ClickCallback clickCallback_;
};

class Button : public ButtonBase
{
public:
	static Button* Create(const char* _text = nullptr);
};

} // namespace sgui
