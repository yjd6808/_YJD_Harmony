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
	pBorder->borderVisual_.Attach(pBorder, -50);
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

void Border::SetBorderBrush(const BrushPtr& _brush)
{
	borderVisual_.SetBrush(_brush);
	borderVisual_.Resize(renderSize_);
}

void Border::SetBorderThickness(const Thickness& _thickness)
{
	borderThickness_ = _thickness;
	borderVisual_.SetThickness(_thickness);
	InvalidateLayout();
}

void Border::RefreshThemeVisuals()
{
	backgroundVisual_.RefreshTheme();
	borderVisual_.RefreshTheme();
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
	borderVisual_.Resize(_size);
}

} // namespace sgui
