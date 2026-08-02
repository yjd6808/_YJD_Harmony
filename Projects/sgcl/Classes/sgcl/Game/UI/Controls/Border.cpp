/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/Border.h"

USING_NS_CC;

namespace sgui
{

Border* Border::Create()
{
	Border* pBorder = dbg_new Border;
	pBorder->autorelease();
	pBorder->backgroundVisual_.Attach(pBorder, -100);
	return pBorder;
}

void Border::SetChild(UIElement* _pChild)
{
	if (pChild_ == _pChild)
	{
		return;
	}

	if (pChild_)
	{
		pChild_->SetParentElement(nullptr);
		pChild_->removeFromParent();
	}

	pChild_ = _pChild;

	if (pChild_)
	{
		pChild_->SetParentElement(this);
		addChild(pChild_);
	}

	InvalidateLayout();
}

void Border::SetBackground(const BrushPtr& _brush)
{
	backgroundVisual_.SetBrush(_brush);
	backgroundVisual_.Resize(renderSize_);
}

void Border::SetBorderBrush(const UIColorF& _color)
{
	borderColor_ = _color;
	SyncBorderEdges();
}

void Border::SetBorderThickness(const Thickness& _thickness)
{
	borderThickness_ = _thickness;
	SyncBorderEdges();
	InvalidateLayout();
}

void Border::RefreshThemeVisuals()
{
	backgroundVisual_.RefreshTheme();
	UIElement::RefreshThemeVisuals();
}

cc::size Border::MeasureOverride(const cc::size& _constraint)
{
	const float extraW = borderThickness_.Width() + padding_.Width();
	const float extraH = borderThickness_.Height() + padding_.Height();

	if (pChild_ == nullptr)
	{
		return cc::size(extraW, extraH);
	}

	pChild_->Measure(cc::size(
		std::max(0.0f, _constraint.width - extraW),
		std::max(0.0f, _constraint.height - extraH)));

	const cc::size& desired = pChild_->GetDesiredSize();
	return cc::size(desired.width + extraW, desired.height + extraH);
}

void Border::ArrangeOverride(const cc::size& _finalSize)
{
	if (pChild_ == nullptr)
	{
		return;
	}

	pChild_->Arrange(Rect(
		borderThickness_.left_ + padding_.left_,
		borderThickness_.top_ + padding_.top_,
		std::max(0.0f, _finalSize.width - borderThickness_.Width() - padding_.Width()),
		std::max(0.0f, _finalSize.height - borderThickness_.Height() - padding_.Height())));
}

void Border::OnRenderSizeChanged(const cc::size& _size)
{
	backgroundVisual_.Resize(_size);
	SyncBorderEdges();
}

void Border::SyncBorderEdges()
{
	const Color4B color = ToColor4B(borderColor_);
	const float width = renderSize_.width;
	const float height = renderSize_.height;

	const float thickness[4] = { borderThickness_.left_, borderThickness_.top_, borderThickness_.right_, borderThickness_.bottom_ };

	for (int idx = 0; idx < 4; ++idx)
	{
		if (thickness[idx] <= 0.0f)
		{
			if (pEdges_[idx])
			{
				pEdges_[idx]->setVisible(false);
			}
			continue;
		}

		if (pEdges_[idx] == nullptr)
		{
			pEdges_[idx] = LayerColor::create(color, 1.0f, 1.0f);
			addChild(pEdges_[idx], -50);
		}

		pEdges_[idx]->setColor(Color3B(color.r, color.g, color.b));
		pEdges_[idx]->setOpacity(color.a);
		pEdges_[idx]->setVisible(true);
	}

	// left, top, right, bottom (코코스 y-up 좌표로 배치)
	if (pEdges_[0]) { pEdges_[0]->setContentSize(cc::size(thickness[0], height)); pEdges_[0]->setPosition(0.0f, 0.0f); }
	if (pEdges_[1]) { pEdges_[1]->setContentSize(cc::size(width, thickness[1])); pEdges_[1]->setPosition(0.0f, height - thickness[1]); }
	if (pEdges_[2]) { pEdges_[2]->setContentSize(cc::size(thickness[2], height)); pEdges_[2]->setPosition(width - thickness[2], 0.0f); }
	if (pEdges_[3]) { pEdges_[3]->setContentSize(cc::size(width, thickness[3])); pEdges_[3]->setPosition(0.0f, 0.0f); }
}

} // namespace sgui
