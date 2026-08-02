/*
 * 작성자: 윤정도
 * =====================
 * sgui - 단일 Content를 가지는 컨트롤 (WPF ContentControl)
 * WPF의 Button.Content처럼 임의의 UIElement를 내용으로 가질 수 있다.
 * SetContentText()는 TextBlock을 생성해 Content로 설정하는 편의 함수다.
 */

#pragma once

#include "sgcl/Game/UI/Controls/Control.h"

namespace sgui
{

class TextBlock;

class ContentControl : public Control
{
public:
	void SetContent(UIElement* _pContent);
	UIElement* GetContent() const { return pContent_; }

	// 편의 함수: 문자열을 TextBlock으로 감싸서 Content로 설정한다.
	TextBlock* SetContentText(const char* _text);

	void SetContentAlignment(HorizontalAlignment _horizontal, VerticalAlignment _vertical);

	virtual int GetChildElementCount() const override { return pContent_ ? 1 : 0; }
	virtual UIElement* GetChildElementAt(int _index) const override { return _index == 0 ? pContent_ : nullptr; }

protected:
	virtual cc::size MeasureOverride(const cc::size& _constraint) override;
	virtual void ArrangeOverride(const cc::size& _finalSize) override;

	UIElement* pContent_ = nullptr;
	HorizontalAlignment contentHAlignment_ = HorizontalAlignment::Center;
	VerticalAlignment contentVAlignment_ = VerticalAlignment::Center;
};

} // namespace sgui
