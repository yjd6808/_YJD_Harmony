/*
 * 작성자: 윤정도
 * =====================
 * sgui - 텍스트 표시 요소 (WPF TextBlock)
 * Foreground/FontCode/FontSize는 상속 속성을 따른다. (명시 설정 시 자신 값 사용)
 */

#pragma once

#include "sgcl/Game/UI/Core/UIElement.h"
#include "jc/Primitives/String.h"

namespace sgui
{

class TextBlock : public UIElement
{
public:
	static TextBlock* Create(const char* _text = "");

	void SetText(const char* _text);
	const jc::String& GetText() const { return text_; }

	void SetTextAlignment(TextAlignment _alignment);
	void SetTextWrapping(TextWrapping _wrapping);

	// 텍스트는 기본적으로 히트테스트 대상이 아니다. (WPF와 다르지만 게임 UI 관례상 편의)
	virtual bool IsSelfHitTestVisible() const override { return isHitTestVisible_ && hitTestEnabled_; }
	void SetHitTestEnabled(bool _enabled) { hitTestEnabled_ = _enabled; }

	virtual void RefreshThemeVisuals() override;
	virtual void RefreshForegroundVisuals() override;
	virtual void UpdateVisualState() override;

protected:
	virtual cc::size MeasureOverride(const cc::size& _constraint) override;
	virtual void ArrangeOverride(const cc::size& _finalSize) override;
	virtual void OnInheritedPropertyChanged() override;

	void RebuildLabel();
	void SyncLabelStyle();

	jc::String text_;
	TextAlignment textAlignment_ = TextAlignment::Left;
	TextWrapping textWrapping_ = TextWrapping::NoWrap;
	bool hitTestEnabled_ = false;

	cc::Label* pLabel_ = nullptr;
	int builtFontCode_ = -1;
};

} // namespace sgui
