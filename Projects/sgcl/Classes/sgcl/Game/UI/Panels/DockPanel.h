/*
 * 작성자: 윤정도
 * =====================
 * sgui - 가장자리 부착 패널 (WPF DockPanel)
 */

#pragma once

#include "sgcl/Game/UI/Panels/Panel.h"

namespace sgui
{

class DockPanel : public Panel
{
public:
	static DockPanel* Create(bool _lastChildFill = true)
	{
		DockPanel* pPanel = dbg_new DockPanel;
		pPanel->lastChildFill_ = _lastChildFill;
		pPanel->autorelease();
		return pPanel;
	}

	void SetLastChildFill(bool _fill) { lastChildFill_ = _fill; InvalidateLayout(); }

protected:
	cc::size MeasureOverride(const cc::size& _constraint) override
	{
		float usedWidth = 0.0f;
		float usedHeight = 0.0f;
		float desiredWidth = 0.0f;
		float desiredHeight = 0.0f;

		for (size_t i = 0; i < children_.size(); ++i)
		{
			UIElement* pChild = children_[i];

			cc::size remain(
				std::max(0.0f, _constraint.width - usedWidth),
				std::max(0.0f, _constraint.height - usedHeight));

			pChild->Measure(remain);

			if (pChild->GetVisibility() == Visibility::Collapsed)
				continue;

			const cc::size& desired = pChild->GetDesiredSize();

			switch (pChild->GetDock())
			{
			case Dock::Left:
			case Dock::Right:
				desiredHeight = std::max(desiredHeight, usedHeight + desired.height);
				usedWidth += desired.width;
				desiredWidth = std::max(desiredWidth, usedWidth);
				break;
			case Dock::Top:
			case Dock::Bottom:
				desiredWidth = std::max(desiredWidth, usedWidth + desired.width);
				usedHeight += desired.height;
				desiredHeight = std::max(desiredHeight, usedHeight);
				break;
			}
		}

		return cc::size(desiredWidth, desiredHeight);
	}

	void ArrangeOverride(const cc::size& _finalSize) override
	{
		float left = 0.0f;
		float top = 0.0f;
		float right = _finalSize.width;
		float bottom = _finalSize.height;

		for (size_t i = 0; i < children_.size(); ++i)
		{
			UIElement* pChild = children_[i];

			if (pChild->GetVisibility() == Visibility::Collapsed)
			{
				pChild->Arrange(Rect(0.0f, 0.0f, 0.0f, 0.0f));
				continue;
			}

			const bool isLast = i == children_.size() - 1;
			const cc::size& desired = pChild->GetDesiredSize();
			const float remainWidth = std::max(0.0f, right - left);
			const float remainHeight = std::max(0.0f, bottom - top);

			if (isLast && lastChildFill_)
			{
				pChild->Arrange(Rect(left, top, remainWidth, remainHeight));
				break;
			}

			switch (pChild->GetDock())
			{
			case Dock::Left:
				pChild->Arrange(Rect(left, top, std::min(desired.width, remainWidth), remainHeight));
				left += desired.width;
				break;
			case Dock::Right:
				pChild->Arrange(Rect(std::max(left, right - desired.width), top, std::min(desired.width, remainWidth), remainHeight));
				right -= desired.width;
				break;
			case Dock::Top:
				pChild->Arrange(Rect(left, top, remainWidth, std::min(desired.height, remainHeight)));
				top += desired.height;
				break;
			case Dock::Bottom:
				pChild->Arrange(Rect(left, std::max(top, bottom - desired.height), remainWidth, std::min(desired.height, remainHeight)));
				bottom -= desired.height;
				break;
			}
		}
	}

	bool lastChildFill_ = true;
};

} // namespace sgui
