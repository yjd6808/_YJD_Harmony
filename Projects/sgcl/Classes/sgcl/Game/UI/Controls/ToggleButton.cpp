/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/ToggleButton.h"
#include "sgcl/Game/UI/Controls/Window.h"

namespace sgui
{

// ==================== ToggleButton ====================

ToggleButton* ToggleButton::Create(const char* _text /* = nullptr */)
{
	ToggleButton* pToggle = dbg_new ToggleButton;
	pToggle->autorelease();
	pToggle->SetBackground(ThemeBrush::Create(UIAssetSemantic::Button, UIElementType::ToggleButton));
	pToggle->SetPadding(Thickness(12.0f, 6.0f));

	if (_text)
	{
		pToggle->SetContentText(_text);
	}

	return pToggle;
}

void ToggleButton::SetIsChecked(bool _checked)
{
	if (isChecked_ == _checked)
	{
		return;
	}

	isChecked_ = _checked;
	OnCheckedChanged();

	UIEventChecked event;
	event.type_ = ueCheckedChanged;
	event.checked_ = isChecked_;
	RaiseEvent(&event);
}

void ToggleButton::OnClick()
{
	SetIsChecked(!isChecked_);
	ButtonBase::OnClick();
}

void ToggleButton::OnCheckedChanged()
{
	UpdateVisualState();
}

UIVisualState ToggleButton::MapVisualState() const
{
	if (!IsEffectivelyEnabled())
		return UIVisualState::Disabled;

	if (isPressed_)
		return UIVisualState::Pressed;

	if (isChecked_)
		return UIVisualState::Checked;

	if (isMouseOver_)
		return UIVisualState::Hover;

	return UIVisualState::Normal;
}

void ToggleButton::BuildSwitchVisuals()
{
	useSwitchVisuals_ = true;

	trackVisual_.Attach(this, -90);
	trackVisual_.SetBrush(ThemeBrush::Create(UIAssetSemantic::ToggleTrack, UIElementType::ToggleButton, UIComponentSlot::Track));

	knobVisual_.Attach(this, -80);
	knobVisual_.SetBrush(ThemeBrush::Create(UIAssetSemantic::ToggleKnob, UIElementType::ToggleButton, UIComponentSlot::Knob, true));
}

void ToggleButton::RefreshThemeVisuals()
{
	trackVisual_.RefreshTheme();
	knobVisual_.RefreshTheme();
	ButtonBase::RefreshThemeVisuals();
}

void ToggleButton::OnRenderSizeChanged(const cc::size& _size)
{
	ButtonBase::OnRenderSizeChanged(_size);

	if (!useSwitchVisuals_)
	{
		return;
	}

	trackVisual_.Resize(_size);
	knobVisual_.Resize(_size);

	// 노브는 체크 상태에 따라 좌/우로 이동한다.
	if (cc::Node* pKnob = knobVisual_.GetNode())
	{
		const float knobHalf = _size.height * 0.5f;
		pKnob->setPosition(isChecked_ ? _size.width - knobHalf : knobHalf, _size.height * 0.5f);
	}
}

// ==================== CheckBox ====================

CheckBox* CheckBox::Create(const char* _text /* = nullptr */)
{
	CheckBox* pCheckBox = dbg_new CheckBox;
	pCheckBox->autorelease();
	pCheckBox->SetContentAlignment(HorizontalAlignment::Left, VerticalAlignment::Center);

	pCheckBox->boxVisual_.Attach(pCheckBox, -90);
	pCheckBox->boxVisual_.SetBrush(ThemeBrush::Create(UIAssetSemantic::CheckBox, UIElementType::CheckBox));

	pCheckBox->markVisual_.Attach(pCheckBox, -80);
	pCheckBox->markVisual_.SetBrush(ThemeBrush::Create(UIAssetSemantic::CheckMark, UIElementType::CheckBox, UIComponentSlot::Mark, true));

	if (_text)
	{
		pCheckBox->SetContentText(_text);
	}

	return pCheckBox;
}

void CheckBox::RefreshThemeVisuals()
{
	boxVisual_.RefreshTheme();
	markVisual_.RefreshTheme();
	ToggleButton::RefreshThemeVisuals();
}

void CheckBox::OnCheckedChanged()
{
	ToggleButton::OnCheckedChanged();

	boxVisual_.SetVisualState(MapVisualState());
	markVisual_.SetVisualState(MapVisualState());

	if (cc::Node* pMark = markVisual_.GetNode())
	{
		pMark->setVisible(isChecked_);
	}
}

cc::size CheckBox::MeasureOverride(const cc::size& _constraint)
{
	cc::size contentSize(0.0f, 0.0f);

	if (pContent_)
	{
		pContent_->Measure(cc::size(
			std::max(0.0f, _constraint.width - BoxSize_v - BoxGap_v),
			_constraint.height));
		contentSize = pContent_->GetDesiredSize();
	}

	return cc::size(
		BoxSize_v + (contentSize.width > 0.0f ? BoxGap_v + contentSize.width : 0.0f),
		std::max(BoxSize_v, contentSize.height));
}

void CheckBox::ArrangeOverride(const cc::size& _finalSize)
{
	if (pContent_)
	{
		pContent_->Arrange(Rect(
			BoxSize_v + BoxGap_v,
			0.0f,
			std::max(0.0f, _finalSize.width - BoxSize_v - BoxGap_v),
			_finalSize.height));
	}
}

void CheckBox::OnRenderSizeChanged(const cc::size& _size)
{
	Control::OnRenderSizeChanged(_size);

	const float boxY = (_size.height - BoxSize_v) * 0.5f;

	boxVisual_.Resize(cc::size(BoxSize_v, BoxSize_v));
	markVisual_.Resize(cc::size(BoxSize_v, BoxSize_v));

	if (cc::Node* pBox = boxVisual_.GetNode())
	{
		pBox->setPosition(0.0f, boxY);
	}

	if (cc::Node* pMark = markVisual_.GetNode())
	{
		pMark->setPosition(BoxSize_v * 0.5f, boxY + BoxSize_v * 0.5f);
		pMark->setVisible(isChecked_);
	}
}

// ==================== RadioButton ====================

RadioButton* RadioButton::Create(const char* _text /* = nullptr */, const char* _groupName /* = "" */)
{
	RadioButton* pRadio = dbg_new RadioButton;
	pRadio->autorelease();
	pRadio->groupName_ = _groupName ? _groupName : "";
	pRadio->SetContentAlignment(HorizontalAlignment::Left, VerticalAlignment::Center);

	pRadio->radioVisual_.Attach(pRadio, -90);
	pRadio->radioVisual_.SetBrush(ThemeBrush::Create(UIAssetSemantic::Radio, UIElementType::CheckBox));

	pRadio->dotVisual_.Attach(pRadio, -80);
	pRadio->dotVisual_.SetBrush(ThemeBrush::Create(UIAssetSemantic::RadioDot, UIElementType::CheckBox, UIComponentSlot::Dot, true));

	if (_text)
	{
		pRadio->SetContentText(_text);
	}

	return pRadio;
}

void RadioButton::RefreshThemeVisuals()
{
	radioVisual_.RefreshTheme();
	dotVisual_.RefreshTheme();
	ToggleButton::RefreshThemeVisuals();
}

void RadioButton::OnClick()
{
	// 라디오 버튼은 다시 클릭해도 해제되지 않는다.
	if (!isChecked_)
	{
		UncheckGroupSiblings();
		SetIsChecked(true);
	}

	ButtonBase::OnClick();
}

void RadioButton::OnCheckedChanged()
{
	ToggleButton::OnCheckedChanged();

	radioVisual_.SetVisualState(MapVisualState());
	dotVisual_.SetVisualState(MapVisualState());

	if (cc::Node* pDot = dotVisual_.GetNode())
	{
		pDot->setVisible(isChecked_);
	}
}

void RadioButton::UncheckGroupSiblings()
{
	Window* pWindow = GetWindow();

	if (pWindow == nullptr)
	{
		return;
	}

	pWindow->ForEachRecursiveContainedSelf([this](UIElement* _pElement)
	{
		auto* pRadio = dynamic_cast<RadioButton*>(_pElement);

		if (pRadio == nullptr || pRadio == this)
			return;

		if (pRadio->groupName_ == groupName_)
		{
			pRadio->SetIsChecked(false);
		}
	});
}

cc::size RadioButton::MeasureOverride(const cc::size& _constraint)
{
	cc::size contentSize(0.0f, 0.0f);

	if (pContent_)
	{
		pContent_->Measure(cc::size(
			std::max(0.0f, _constraint.width - RadioSize_v - RadioGap_v),
			_constraint.height));
		contentSize = pContent_->GetDesiredSize();
	}

	return cc::size(
		RadioSize_v + (contentSize.width > 0.0f ? RadioGap_v + contentSize.width : 0.0f),
		std::max(RadioSize_v, contentSize.height));
}

void RadioButton::ArrangeOverride(const cc::size& _finalSize)
{
	if (pContent_)
	{
		pContent_->Arrange(Rect(
			RadioSize_v + RadioGap_v,
			0.0f,
			std::max(0.0f, _finalSize.width - RadioSize_v - RadioGap_v),
			_finalSize.height));
	}
}

void RadioButton::OnRenderSizeChanged(const cc::size& _size)
{
	Control::OnRenderSizeChanged(_size);

	const float radioY = (_size.height - RadioSize_v) * 0.5f;

	radioVisual_.Resize(cc::size(RadioSize_v, RadioSize_v));
	dotVisual_.Resize(cc::size(RadioSize_v, RadioSize_v));

	if (cc::Node* pRadio = radioVisual_.GetNode())
	{
		pRadio->setPosition(0.0f, radioY);
	}

	if (cc::Node* pDot = dotVisual_.GetNode())
	{
		pDot->setPosition(RadioSize_v * 0.5f, radioY + RadioSize_v * 0.5f);
		pDot->setVisible(isChecked_);
	}
}

} // namespace sgui
