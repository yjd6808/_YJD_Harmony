/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 5:54:04 PM
 * =====================
 *
 */



#include "Tutturu.h"
#include "UILabel.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/UIMasterGroup.h>

USING_NS_CC;
USING_NS_JC;

UILabel::UILabel(UIMasterGroup* master, UIGroup* parent)
	: UIElement(master, parent)
	, m_fInitialFontSize(12.0f)
	, m_fFontSize(12.0f)
	, m_bFontAutoScaling(true)
	, m_pInfo( nullptr )
	, m_pLabel{ nullptr }
{}

UILabel::UILabel(UIMasterGroup* master, UIGroup* parent, UILabelInfo* labelInfo, bool infoOwner)
	: UIElement(master, parent, labelInfo, infoOwner)
	, m_fInitialFontSize(12.0f)
	, m_fFontSize(12.0f)
	, m_bFontAutoScaling(true)
	, m_pInfo(labelInfo)
	, m_pLabel{nullptr}
{}

UILabel::~UILabel() {
}

UILabel* UILabel::create(UIMasterGroup* master, UIGroup* parent) {
	UILabel* pLabel = dbg_new UILabel(master, parent);
	pLabel->init();
	pLabel->autorelease();
	return pLabel;
}

UILabel* UILabel::create(UIMasterGroup* master, UIGroup* parent, UILabelInfo* labelInfo, bool infoOwner) {
	UILabel* pLabel = dbg_new UILabel(master, parent, labelInfo, infoOwner);
	pLabel->init();
	pLabel->autorelease();
	return pLabel;
}

SGString UILabel::getFontPath() const {
	const SGString fontName = CoreFont_v->getFontName(m_pInfo->FontCode);
	const SGString fontPath = Path::Combine(CoreCommonInfo_v->DataPath, Const::Resource::FontDirName, fontName);
	return fontPath;
}

void UILabel::setText(const std::string& text) {
	m_pLabel->setString(text);
}

void UILabel::setText(const std::string& text, float fontSize) {
	m_pLabel->initWithTTF(text, getFontPath().Source(), fontSize);
	setContentSize({ m_pInfo->Size.width, m_pInfo->Size.height });
}

void UILabel::setText(const std::string& text, float fontSize, const SGSize& dimension) {
	m_pLabel->initWithTTF(text, getFontPath().Source(), fontSize);
	setContentSize(dimension);
}

void UILabel::setUISize(const SGSize& contentSize) {
	if (!m_bResizable)
		return;

	m_UISize = contentSize;

	if (m_pLabel == nullptr)
		return;

	// TODO: (완료) 폰트 사이즈도 변경되야함.
	if (m_bFontAutoScaling) {
		m_fFontSize = m_fInitialFontSize * getUIScaleY();
		m_pLabel->initWithTTF(m_pLabel->getString(), getFontPath().Source(), (int)m_fFontSize, { m_UISize.width, m_UISize.height });
		m_pLabel->setLineHeight((int)m_fFontSize);
	} else {
		m_pLabel->setDimensions(m_UISize.width, m_UISize.height);
	}
}

void UILabel::setInfo(UIElementInfo* info, bool infoOwner) {
	if (info->Type != UIElementType::Label) {
		logWarnInvalidInfo(info->Type);
		return;
	}

	if (m_bInfoOwner) {
		JCORE_DELETE_SAFE(m_pInfo);
	}

	m_pBaseInfo = info;
	m_pInfo = static_cast<UILabelInfo*>(info);
	m_bInfoOwner = infoOwner;
}

void UILabel::setInfoLabel(UILabelInfo* info, bool infoOwner) {
	setInfo(info, infoOwner);
}

void UILabel::setVAlignment(VAlignment_t valign) {
	m_pLabel->setVerticalAlignment((TextVAlignment)valign);
}

void UILabel::setHAlignment(HAlignment_t halign) {
	m_pLabel->setHorizontalAlignment((TextHAlignment)halign);
}




float UILabel::getAppliedFontSize() const {
	return m_pLabel->getTTFConfig().fontSize;
}

int UILabel::getLineCount() const {
	return m_pLabel->getStringNumLines();
}

bool UILabel::init() {

	if (!UIElement::init()) {
		return false;
	}

	if (m_pInfo == nullptr) {
		logWarnMissingInfo();
		return false;
	}

	setInitialUISize(m_pInfo->Size);

	m_pLabel = SGLabel::createWithTTF(m_pInfo->Text.ToStd(), getFontPath().Source(), (int)m_fFontSize, Size::ZERO);
	m_pLabel->setHorizontalAlignment((TextHAlignment)m_pInfo->TextHAlignment);
	m_pLabel->setVerticalAlignment((TextVAlignment)m_pInfo->TextVAlignment);
	m_pLabel->setDimensions(m_UISize.width, m_UISize.height);
	m_pLabel->setTextColor(m_pInfo->FontColor);
	m_pLabel->enableWrap(m_pInfo->TextWrap);
	m_pLabel->setLineHeight((int)m_fFontSize);
	m_pLabel->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pLabel);

	
	return m_bInitialized = true;
}


void UILabel::setInitialUISize(SGSize size) {
	UIElement::setInitialUISize(size);
	m_fFontSize = m_pInfo->FontSize * CoreClientInfo_v->UIScaleYFactor;
	m_fInitialFontSize = m_fFontSize;
}