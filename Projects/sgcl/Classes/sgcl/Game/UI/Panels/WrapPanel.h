/*
 * 작성자: 윤정도
 * =====================
 * sgui - 줄바꿈 패널 (WPF WrapPanel)
 */

#pragma once

#include "sgcl/Game/UI/Panels/Panel.h"

namespace sgui
{

class WrapPanel : public Panel
{
public:
	static WrapPanel* Create(Orientation _orientation = Orientation::Horizontal)
	{
		WrapPanel* pPanel = dbg_new WrapPanel;
		pPanel->orientation_ = _orientation;
		pPanel->autorelease();
		return pPanel;
	}

	void SetOrientation(Orientation _orientation) { orientation_ = _orientation; InvalidateLayout(); }
	void SetItemSize(float _width, float _height) { itemWidth_ = _width; itemHeight_ = _height; InvalidateLayout(); }
	void SetSpacing(float _horizontal, float _vertical) { hSpacing_ = _horizontal; vSpacing_ = _vertical; InvalidateLayout(); }

protected:
	float ItemWidthOf(UIElement* _pChild) const { return !IsAutoSize(itemWidth_) ? itemWidth_ : _pChild->GetDesiredSize().width; }
	float ItemHeightOf(UIElement* _pChild) const { return !IsAutoSize(itemHeight_) ? itemHeight_ : _pChild->GetDesiredSize().height; }

	cc::size MeasureOverride(const cc::size& _constraint) override
	{
		const cc::size childConstraint(
			!IsAutoSize(itemWidth_) ? itemWidth_ : _constraint.width,
			!IsAutoSize(itemHeight_) ? itemHeight_ : _constraint.height);

		float lineMain = 0.0f;
		float lineCross = 0.0f;
		float totalMain = 0.0f;
		float totalCross = 0.0f;

		const bool horizontal = orientation_ == Orientation::Horizontal;
		const float limit = horizontal ? _constraint.width : _constraint.height;
		const float mainSpacing = horizontal ? hSpacing_ : vSpacing_;
		const float crossSpacing = horizontal ? vSpacing_ : hSpacing_;

		for (size_t i = 0; i < children_.size(); ++i)
		{
			UIElement* pChild = children_[i];
			pChild->Measure(childConstraint);

			if (pChild->GetVisibility() == Visibility::Collapsed)
				continue;

			const float childMain = horizontal ? ItemWidthOf(pChild) : ItemHeightOf(pChild);
			const float childCross = horizontal ? ItemHeightOf(pChild) : ItemWidthOf(pChild);
			const float appended = lineMain > 0.0f ? lineMain + mainSpacing + childMain : childMain;

			if (lineMain > 0.0f && appended > limit)
			{
				// 줄바꿈
				totalMain = std::max(totalMain, lineMain);
				totalCross += (totalCross > 0.0f ? crossSpacing : 0.0f) + lineCross;
				lineMain = childMain;
				lineCross = childCross;
			}
			else
			{
				lineMain = appended;
				lineCross = std::max(lineCross, childCross);
			}
		}

		totalMain = std::max(totalMain, lineMain);
		totalCross += (totalCross > 0.0f ? crossSpacing : 0.0f) + lineCross;

		return horizontal ? cc::size(totalMain, totalCross) : cc::size(totalCross, totalMain);
	}

	void ArrangeOverride(const cc::size& _finalSize) override
	{
		const bool horizontal = orientation_ == Orientation::Horizontal;
		const float limit = horizontal ? _finalSize.width : _finalSize.height;
		const float mainSpacing = horizontal ? hSpacing_ : vSpacing_;
		const float crossSpacing = horizontal ? vSpacing_ : hSpacing_;

		float mainPos = 0.0f;
		float crossPos = 0.0f;
		float lineCross = 0.0f;

		for (size_t i = 0; i < children_.size(); ++i)
		{
			UIElement* pChild = children_[i];

			if (pChild->GetVisibility() == Visibility::Collapsed)
			{
				pChild->Arrange(Rect(0.0f, 0.0f, 0.0f, 0.0f));
				continue;
			}

			const float childWidth = ItemWidthOf(pChild);
			const float childHeight = ItemHeightOf(pChild);
			const float childMain = horizontal ? childWidth : childHeight;
			const float childCross = horizontal ? childHeight : childWidth;

			if (mainPos > 0.0f && mainPos + mainSpacing + childMain > limit)
			{
				mainPos = 0.0f;
				crossPos += lineCross + crossSpacing;
				lineCross = 0.0f;
			}
			else if (mainPos > 0.0f)
			{
				mainPos += mainSpacing;
			}

			if (horizontal)
			{
				pChild->Arrange(Rect(mainPos, crossPos, childWidth, childHeight));
			}
			else
			{
				pChild->Arrange(Rect(crossPos, mainPos, childWidth, childHeight));
			}

			mainPos += childMain;
			lineCross = std::max(lineCross, childCross);
		}
	}

	Orientation orientation_ = Orientation::Horizontal;
	float itemWidth_ = AutoSize_v;
	float itemHeight_ = AutoSize_v;
	float hSpacing_ = 0.0f;
	float vSpacing_ = 0.0f;
};

} // namespace sgui
