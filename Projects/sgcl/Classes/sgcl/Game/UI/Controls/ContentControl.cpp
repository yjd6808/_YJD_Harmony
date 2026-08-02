/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/ContentControl.h"
#include "sgcl/Game/UI/Controls/TextBlock.h"

#include <algorithm>

namespace sgui
{

void ContentControl::SetContent(UIElement* _pContent)
{
	if (pContent_ == _pContent)
	{
		return;
	}

	if (pContent_)
	{
		pContent_->SetParentElement(nullptr);
		pContent_->removeFromParent();
	}

	pContent_ = _pContent;

	if (pContent_)
	{
		pContent_->SetParentElement(this);
		addChild(pContent_);
	}

	InvalidateLayout();
}

TextBlock* ContentControl::SetContentText(const char* _text)
{
	TextBlock* pTextBlock = dynamic_cast<TextBlock*>(pContent_);

	if (pTextBlock)
	{
		pTextBlock->SetText(_text);
		return pTextBlock;
	}

	pTextBlock = TextBlock::Create(_text);
	pTextBlock->SetHorizontalAlignment(contentHAlignment_);
	pTextBlock->SetVerticalAlignment(contentVAlignment_);
	SetContent(pTextBlock);
	return pTextBlock;
}

void ContentControl::SetContentAlignment(HorizontalAlignment _horizontal, VerticalAlignment _vertical)
{
	contentHAlignment_ = _horizontal;
	contentVAlignment_ = _vertical;

	if (pContent_)
	{
		pContent_->SetHorizontalAlignment(_horizontal);
		pContent_->SetVerticalAlignment(_vertical);
	}

	InvalidateLayout();
}

cc::size ContentControl::MeasureOverride(const cc::size& _constraint)
{
	const float paddingW = padding_.Width();
	const float paddingH = padding_.Height();

	if (pContent_ == nullptr)
	{
		return cc::size(paddingW, paddingH);
	}

	pContent_->Measure(cc::size(
		std::max(0.0f, _constraint.width - paddingW),
		std::max(0.0f, _constraint.height - paddingH)));

	const cc::size& desired = pContent_->GetDesiredSize();
	return cc::size(desired.width + paddingW, desired.height + paddingH);
}

void ContentControl::ArrangeOverride(const cc::size& _finalSize)
{
	if (pContent_ == nullptr)
	{
		return;
	}

	pContent_->Arrange(Rect(
		padding_.left_,
		padding_.top_,
		std::max(0.0f, _finalSize.width - padding_.Width()),
		std::max(0.0f, _finalSize.height - padding_.Height())));
}

} // namespace sgui
