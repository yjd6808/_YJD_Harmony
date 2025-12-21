/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 5:54:04 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "UILabel.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/UIRootGroup.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UILabel::UILabel(UIRootGroup* _pRoot, UIGroup* _pParent)
: UIElement(_pRoot, _pParent)
, initialFontSize_(12.0f)
, fontSize_(12.0f)
, fontAutoScaling_(true)
, pInfo_(nullptr)
, pLabel_(nullptr)
{
}

UILabel::UILabel(UIRootGroup* _pRoot, UIGroup* _pParent, UILabelInfo* _pLabelInfo, bool _infoOwner)
: UIElement(_pRoot, _pParent, _pLabelInfo, _infoOwner)
, initialFontSize_(12.0f)
, fontSize_(12.0f)
, fontAutoScaling_(true)
, pInfo_(_pLabelInfo)
, pLabel_(nullptr)
{
}

UILabel::~UILabel()
{
}

UILabel* UILabel::create(UIRootGroup* _pRoot, UIGroup* _pParent)
{
	UILabel* pLabel = dbg_new UILabel(_pRoot, _pParent);
	pLabel->init();
	pLabel->autorelease();
	return pLabel;
}

UILabel* UILabel::create(UIRootGroup* _pRoot, UIGroup* _pParent, UILabelInfo* _pLabelInfo, bool _infoOwner)
{
	UILabel* pLabel = dbg_new UILabel(_pRoot, _pParent, _pLabelInfo, _infoOwner);
	pLabel->init();
	pLabel->autorelease();
	return pLabel;
}

SGString UILabel::getFontPath() const
{
	const SGString fontName = Core::Contents.FontManager->GetFontName(pInfo_->fontCode_);
	const SGString fontPath = Path::Combine(Core::CommonInfo->dataPath_, Const::Resource::FontDirName, fontName);
	return fontPath;
}

void UILabel::setText(const std::string& _text)
{
	pLabel_->setString(_text);
}

void UILabel::setText(const std::string& _text, float _fontSize)
{
	pLabel_->initWithTTF(_text, getFontPath().Source(), _fontSize);
	setContentSize({ pInfo_->size_.width, pInfo_->size_.height });
}

void UILabel::setText(const std::string& _text, float _fontSize, const SGSize& _dimension)
{
	pLabel_->initWithTTF(_text, getFontPath().Source(), _fontSize);
	setContentSize(_dimension);
}

void UILabel::SetUISize(const SGSize& _contentSize)
{
	if (!isResizable_)
	{
		return;
	}

	uiSize_ = _contentSize;

	if (pLabel_ == nullptr)
	{
		return;
	}

	// TODO: (완료) 폰트 사이즈도 변경되야함.
	if (fontAutoScaling_)
	{
		fontSize_ = initialFontSize_ * GetUIScaleY();
		pLabel_->initWithTTF(pLabel_->getString(), getFontPath().Source(), static_cast<int>(fontSize_),
		                     { uiSize_.width, uiSize_.height });
		pLabel_->setLineHeight(static_cast<int>(fontSize_));
	}
	else
	{
		pLabel_->setDimensions(uiSize_.width, uiSize_.height);
	}
}

void UILabel::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->type_ != UIElementType::Label)
	{
		LogWarnInvalidInfo(_pInfo->type_);
		return;
	}

	if (isInfoOwner_)
	{
		JCORE_DELETE_SAFE(pInfo_);
	}

	pBaseInfo_ = _pInfo;
	pInfo_ = static_cast<UILabelInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

void UILabel::setInfoLabel(UILabelInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

void UILabel::setVAlignment(VAlignment_t _valign)
{
	pLabel_->setVerticalAlignment((TextVAlignment)_valign);
}

void UILabel::setHAlignment(HAlignment_t _halign)
{
	pLabel_->setHorizontalAlignment((TextHAlignment)_halign);
}

float UILabel::getAppliedFontSize() const
{
	return pLabel_->getTTFConfig().fontSize;
}

int UILabel::getLineCount() const
{
	return pLabel_->getStringNumLines();
}

bool UILabel::init()
{
	if (!UIElement::init())
	{
		return false;
	}

	if (pInfo_ == nullptr)
	{
		LogWarnMissingInfo();
		return false;
	}

	SetInitialUISize(pInfo_->size_);

	pLabel_ = SGLabel::createWithTTF(pInfo_->text_.ToStd(), getFontPath().Source(), static_cast<int>(fontSize_),
	                                 Size::ZERO);
	pLabel_->setHorizontalAlignment((TextHAlignment)pInfo_->textHAlignment_);
	pLabel_->setVerticalAlignment((TextVAlignment)pInfo_->textVAlignment_);
	pLabel_->setDimensions(uiSize_.width, uiSize_.height);
	pLabel_->setTextColor(pInfo_->fontColor_);
	pLabel_->enableWrap(pInfo_->textWrap_);
	pLabel_->setLineHeight(static_cast<int>(fontSize_));
	pLabel_->setAnchorPoint(Vec2::ZERO);
	this->addChild(pLabel_);

	return isInitialized_ = true;
}

void UILabel::SetInitialUISize(SGSize _size)
{
	UIElement::SetInitialUISize(_size);
	fontSize_ = pInfo_->fontSize_ * Core::ClientInfo->uiScaleYFactor_;
	initialFontSize_ = fontSize_;
}
