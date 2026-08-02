/*
 * 작성자: 윤정도
 * =====================
 * sgui - 텍스트 입력 (WPF TextBox)
 * 내부는 cocos EditBox를 사용하며, 변경/제출 시 ueTextChanged / ueTextSubmit(UIEventText)를 발생시킨다.
 */

#pragma once

#include "sgcl/Game/UI/Controls/Control.h"

namespace sgui
{

class TextBox : public Control, public cc_ui::EditBoxDelegate
{
public:
	static TextBox* Create();

	void SetText(const char* _text);
	const char* GetText() const;

	void SetMaxLength(int _maxLength);
	void SetPasswordMode(bool _password);
	void SetPlaceholder(const char* _placeholder);

	virtual void Focus() override;

	virtual void RefreshThemeVisuals() override;
	virtual void RefreshForegroundVisuals() override;

	// cc_ui::EditBoxDelegate
	virtual void editBoxTextChanged(cc_ui::EditBox* _pEditBox, const std::string& _text) override;
	virtual void editBoxReturn(cc_ui::EditBox* _pEditBox) override;

protected:
	TextBox() = default;

	virtual cc::size MeasureOverride(const cc::size& _constraint) override;
	virtual void OnRenderSizeChanged(const cc::size& _size) override;
	virtual void OnInheritedPropertyChanged() override;

	void BuildEditBox();

	cc_ui::EditBox* pEditBox_ = nullptr;
};

} // namespace sgui
