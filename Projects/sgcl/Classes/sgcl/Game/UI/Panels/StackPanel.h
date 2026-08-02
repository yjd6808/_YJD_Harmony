/*
 * 작성자: 윤정도
 * =====================
 * sgui - 한 방향으로 자식을 쌓는 패널 (WPF StackPanel + Spacing 확장)
 */

#pragma once

#include "sgcl/Game/UI/Panels/Panel.h"

namespace sgui
{

class StackPanel : public Panel
{
public:
	static StackPanel* Create(Orientation _orientation = Orientation::Vertical)
	{
		StackPanel* pPanel = dbg_new StackPanel;
		pPanel->orientation_ = _orientation;
		pPanel->autorelease();
		return pPanel;
	}

	void SetOrientation(Orientation _orientation) { orientation_ = _orientation; InvalidateLayout(); }
	Orientation GetOrientation() const { return orientation_; }

	void SetSpacing(float _spacing) { spacing_ = _spacing; InvalidateLayout(); }
	float GetSpacing() const { return spacing_; }

protected:
	cc::size MeasureOverride(const cc::size& _constraint) override
	{
		const bool vertical = orientation_ == Orientation::Vertical;

		cc::size childConstraint = _constraint;

		if (vertical)
		{
			childConstraint.height = Infinity_v;
		}
		else
		{
			childConstraint.width = Infinity_v;
		}

		float mainAxis = 0.0f;
		float crossAxis = 0.0f;
		int visibleCount = 0;

		for (size_t i = 0; i < children_.size(); ++i)
		{
			UIElement* pChild = children_[i];
			pChild->Measure(childConstraint);

			if (pChild->GetVisibility() == Visibility::Collapsed)
				continue;

			const cc::size& desired = pChild->GetDesiredSize();

			if (vertical)
			{
				mainAxis += desired.height;
				crossAxis = std::max(crossAxis, desired.width);
			}
			else
			{
				mainAxis += desired.width;
				crossAxis = std::max(crossAxis, desired.height);
			}

			++visibleCount;
		}

		if (visibleCount > 1)
		{
			mainAxis += spacing_ * (visibleCount - 1);
		}

		return vertical ? cc::size(crossAxis, mainAxis) : cc::size(mainAxis, crossAxis);
	}

	void ArrangeOverride(const cc::size& _finalSize) override
	{
		const bool vertical = orientation_ == Orientation::Vertical;
		float offset = 0.0f;

		for (size_t i = 0; i < children_.size(); ++i)
		{
			UIElement* pChild = children_[i];

			if (pChild->GetVisibility() == Visibility::Collapsed)
			{
				pChild->Arrange(Rect(0.0f, 0.0f, 0.0f, 0.0f));
				continue;
			}

			const cc::size& desired = pChild->GetDesiredSize();

			if (vertical)
			{
				pChild->Arrange(Rect(0.0f, offset, _finalSize.width, desired.height));
				offset += desired.height + spacing_;
			}
			else
			{
				pChild->Arrange(Rect(offset, 0.0f, desired.width, _finalSize.height));
				offset += desired.width + spacing_;
			}
		}
	}

	Orientation orientation_ = Orientation::Vertical;
	float spacing_ = 0.0f;
};

} // namespace sgui
