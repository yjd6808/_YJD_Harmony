/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/Button.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"

namespace sgui
{

ButtonBase::ButtonBase()
{
	focusable_ = true;
}

void ButtonBase::OnEventRaised(int _id, UIEvent* _pEvent)
{
	Control::OnEventRaised(_id, _pEvent);

	switch (_pEvent->type_)
	{
	case ueMouseDown:
	{
		if (_pEvent->phase_ != UIRoutingPhase::Bubble || !IsEffectivelyEnabled())
			break;

		isPressed_ = true;
		CaptureMouse();
		UpdateVisualState();
		_pEvent->handled_ = true;
		break;
	}
	case ueMouseUp:
	{
		if (_pEvent->phase_ != UIRoutingPhase::Bubble || !isPressed_)
			break;

		isPressed_ = false;
		ReleaseMouseCapture();
		UpdateVisualState();
		_pEvent->handled_ = true;

		auto* pMouseEvent = _pEvent->Cast<UIEventMouse>();

		if (IsEffectivelyEnabled() && ContainsWorldPoint(pMouseEvent->screenPos_))
		{
			OnClick();
		}
		break;
	}
	default:
		break;
	}
}

void ButtonBase::OnClick()
{
	UIEvent event;
	event.type_ = ueClick;
	RaiseEvent(&event);

	if (clickCallback_)
	{
		clickCallback_(this);
	}
}

Button* Button::Create(const char* _text /* = nullptr */)
{
	Button* pButton = dbg_new Button;
	pButton->autorelease();
	pButton->SetThemeControl(UIThemeControl::Button);
	pButton->SetBackground(ThemeColorBrush::Create(UIThemeControl::Button, UIThemeColorRole::Background));
	pButton->SetBorderBrush(ThemeColorBrush::Create(UIThemeControl::Button, UIThemeColorRole::Border));
	pButton->SetBorderThickness(Thickness(UIThemeManager::Get()->GetColors().GetBorderThickness(UIThemeControl::Button)));
	pButton->SetPadding(Thickness(12.0f, 6.0f));

	if (_text)
	{
		pButton->SetContentText(_text);
	}

	return pButton;
}

} // namespace sgui
