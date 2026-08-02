/*
 * 작성자: 윤정도
 * =====================
 * sgui - 스크롤 가능 컨텐츠 램퍼 (WPF ScrollViewer)
 * ClippingRectangleNode로 컨텐츠를 자른다. 수직 스크롤만 지원한다.
 */

#pragma once

#include "sgcl/Game/UI/Controls/Control.h"

namespace sgui
{

class ScrollViewer : public Control
{
public:
	static ScrollViewer* Create();

	void SetContent(UIElement* _pContent);
	UIElement* GetContent() const { return pContent_; }

	void SetVerticalOffset(float _offset);
	float GetVerticalOffset() const { return verticalOffset_; }
	float GetScrollableHeight() const;

	void SetWheelStep(float _step) { wheelStep_ = _step; }

	virtual int GetChildElementCount() const override { return pContent_ ? 1 : 0; }
	virtual UIElement* GetChildElementAt(int _index) const override { return _index == 0 ? pContent_ : nullptr; }

	virtual void OnEventRaised(int _id, UIEvent* _pEvent) override;

protected:
	ScrollViewer() = default;

	virtual cc::size MeasureOverride(const cc::size& _constraint) override;
	virtual void ArrangeOverride(const cc::size& _finalSize) override;
	virtual void OnRenderSizeChanged(const cc::size& _size) override;

	void SyncContentPosition();

	UIElement* pContent_ = nullptr;
	cc::ClippingRectangleNode* pClipNode_ = nullptr;
	float verticalOffset_ = 0.0f;
	float wheelStep_ = 40.0f;
};

} // namespace sgui
