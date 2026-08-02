/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/TextBox.h"
#include "sgcl/Game/Contents/FontManager.h"

USING_NS_CC;
USING_NS_CCUI;

namespace sgui
{

TextBox* TextBox::Create()
{
	TextBox* pTextBox = dbg_new TextBox;
	pTextBox->autorelease();
	pTextBox->focusable_ = true;
	pTextBox->SetBackground(ThemeBrush::Create(UIAssetSemantic::EditBox, UIElementType::EditBox));
	pTextBox->BuildEditBox();
	return pTextBox;
}

void TextBox::BuildEditBox()
{
	pEditBox_ = EditBox::create(cc::size(120.0f, 24.0f), Scale9Sprite::create());
	pEditBox_->setAnchorPoint(cc::vec2::ZERO);
	pEditBox_->setDelegate(this);
	pEditBox_->setFontName(g_cFontMgr.GetFontPath(GetEffectiveFontCode()).Source());
	pEditBox_->setFontSize((int)GetEffectiveFontSize());
	pEditBox_->setFontColor(ToColor4B(GetEffectiveForeground()));
	pEditBox_->setInputMode(EditBox::InputMode::SINGLE_LINE);
	addChild(pEditBox_);
}

void TextBox::SetText(const char* _text)
{
	pEditBox_->setText(_text ? _text : "");
}

const char* TextBox::GetText() const
{
	return pEditBox_->getText();
}

void TextBox::SetMaxLength(int _maxLength)
{
	pEditBox_->setMaxLength(_maxLength);
}

void TextBox::SetPasswordMode(bool _password)
{
	pEditBox_->setInputFlag(_password ? EditBox::InputFlag::PASSWORD : EditBox::InputFlag::SENSITIVE);
}

void TextBox::SetPlaceholder(const char* _placeholder)
{
	pEditBox_->setPlaceHolder(_placeholder ? _placeholder : "");
}

void TextBox::Focus()
{
	Control::Focus();

	if (pEditBox_)
	{
		pEditBox_->touchDownAction(pEditBox_, Widget::TouchEventType::ENDED);
	}
}

void TextBox::editBoxTextChanged(EditBox* _pEditBox, const std::string& _text)
{
	UNUSED(_pEditBox);

	UIEventText event;
	event.type_ = ueTextChanged;
	event.text_ = _text;
	RaiseEvent(&event);
}

void TextBox::editBoxReturn(EditBox* _pEditBox)
{
	UNUSED(_pEditBox);

	UIEventText event;
	event.type_ = ueTextSubmit;
	event.text_ = pEditBox_ ? pEditBox_->getText() : "";
	RaiseEvent(&event);
}

cc::size TextBox::MeasureOverride(const cc::size& _constraint)
{
	UNUSED(_constraint);
	return cc::size(120.0f + padding_.Width(), 24.0f + padding_.Height());
}

void TextBox::OnRenderSizeChanged(const cc::size& _size)
{
	Control::OnRenderSizeChanged(_size);

	if (pEditBox_)
	{
		const float innerW = std::max(0.0f, _size.width - padding_.Width());
		const float innerH = std::max(0.0f, _size.height - padding_.Height());
		pEditBox_->setContentSize(cc::size(innerW, innerH));
		pEditBox_->setPosition(cc::vec2(padding_.left_, padding_.bottom_));
	}
}

} // namespace sgui
