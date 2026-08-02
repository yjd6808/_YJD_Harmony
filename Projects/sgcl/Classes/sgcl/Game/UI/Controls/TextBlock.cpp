/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Controls/TextBlock.h"
#include "sgcl/Game/Contents/FontManager.h"

USING_NS_CC;

namespace sgui
{

TextBlock* TextBlock::Create(const char* _text /* = "" */)
{
	TextBlock* pTextBlock = dbg_new TextBlock;
	pTextBlock->text_ = _text ? _text : "";
	pTextBlock->autorelease();
	pTextBlock->RebuildLabel();
	return pTextBlock;
}

void TextBlock::SetText(const char* _text)
{
	const std::string newText = _text ? _text : "";

	if (text_ == newText)
	{
		return;
	}

	text_ = newText;

	if (pLabel_)
	{
		pLabel_->setString(text_);
	}

	InvalidateLayout();
}

void TextBlock::SetTextAlignment(TextAlignment _alignment)
{
	textAlignment_ = _alignment;
	SyncLabelStyle();
}

void TextBlock::SetTextWrapping(TextWrapping _wrapping)
{
	textWrapping_ = _wrapping;
	InvalidateLayout();
}

void TextBlock::RefreshThemeVisuals()
{
	SyncLabelStyle();
	UIElement::RefreshThemeVisuals();
}

void TextBlock::RebuildLabel()
{
	if (pLabel_)
	{
		pLabel_->removeFromParent();
		pLabel_ = nullptr;
	}

	const int fontCode = GetEffectiveFontCode();
	const float fontSize = GetEffectiveFontSize();
	const jc::String fontPath = g_cFontMgr.GetFontPath(fontCode);

	pLabel_ = Label::createWithTTF(text_, fontPath.Source(), fontSize);

	if (pLabel_ == nullptr)
	{
		_LogWarn_("[TextBlock] TTF 폰트 생성 실패(fontCode: %d). 시스템 폰트로 대체합니다.", fontCode);
		pLabel_ = Label::createWithSystemFont(text_, "Arial", fontSize);
	}

	builtFontCode_ = fontCode;
	pLabel_->setAnchorPoint(cc::vec2(0.0f, 1.0f));
	addChild(pLabel_);
	SyncLabelStyle();
}

void TextBlock::SyncLabelStyle()
{
	if (pLabel_ == nullptr)
	{
		return;
	}

	pLabel_->setTextColor(ToColor4B(GetEffectiveForeground()));

	switch (textAlignment_)
	{
	case TextAlignment::Left: pLabel_->setHorizontalAlignment(TextHAlignment::LEFT); break;
	case TextAlignment::Center: pLabel_->setHorizontalAlignment(TextHAlignment::CENTER); break;
	case TextAlignment::Right: pLabel_->setHorizontalAlignment(TextHAlignment::RIGHT); break;
	}
}

cc::size TextBlock::MeasureOverride(const cc::size& _constraint)
{
	if (pLabel_ == nullptr || builtFontCode_ != GetEffectiveFontCode())
	{
		RebuildLabel();
	}

	if (pLabel_ == nullptr)
	{
		return cc::size(0.0f, 0.0f);
	}

	if (textWrapping_ == TextWrapping::Wrap && !std::isinf(_constraint.width))
	{
		pLabel_->setDimensions(_constraint.width, 0.0f);
	}
	else
	{
		pLabel_->setDimensions(0.0f, 0.0f);
	}

	return pLabel_->getContentSize();
}

void TextBlock::ArrangeOverride(const cc::size& _finalSize)
{
	if (pLabel_ == nullptr)
	{
		return;
	}

	// 라벨은 요소 로컬 좌표계(y-up) 기준 좌상단에 부착한다.
	pLabel_->setPosition(0.0f, _finalSize.height);
}

void TextBlock::OnInheritedPropertyChanged()
{
	if (builtFontCode_ != GetEffectiveFontCode())
	{
		RebuildLabel();
	}
	else
	{
		SyncLabelStyle();
	}

	UIElement::OnInheritedPropertyChanged();
}

} // namespace sgui
