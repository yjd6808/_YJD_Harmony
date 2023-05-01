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

UILabel::UILabel(UIMasterGroup* master, UIGroup* parent, UILabelInfo* labelInfo)
	: UIElement(master, parent, labelInfo)
	, m_bFontAutoScaling(true)
	, m_pInfo(labelInfo)
	, m_pLabel{nullptr}
{}

UILabel::~UILabel() {
}

UILabel* UILabel::create(UIMasterGroup* master, UIGroup* parent, UILabelInfo* labelInfo) {
	UILabel* pLabel = dbg_new UILabel(master, parent, labelInfo);
	pLabel->init();
	pLabel->autorelease();
	return pLabel;
}

SGString UILabel::getFontPath() const {
	const SGString fontName = CoreFont_v->getFontName(m_pInfo->FontCode);
	const SGString fontPath = Path::Combine(CoreCommonInfo_v->DataPath, FontDirName_v, fontName);
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

	Size prevSize = m_UISize;
	m_UISize = contentSize;

	if (m_pLabel == nullptr)
		return;

	// TODO: (완료) 폰트 사이즈도 변경되야함.
	if (m_bFontAutoScaling) {
		float fScaleY = m_UISize.height / prevSize.height;
		float fFontSize = getFontSize() * fScaleY;

		m_pLabel->initWithTTF(m_pLabel->getString(), getFontPath().Source(), fFontSize, { m_UISize.width, m_UISize.height });
	} else {
		m_pLabel->setDimensions(m_UISize.width, m_UISize.height);
	}
}

void UILabel::setVAlignment(VAlignment_t valign) {
	m_pLabel->setVerticalAlignment((TextVAlignment)valign);
}

void UILabel::setHAlignment(HAlignment_t halign) {
	m_pLabel->setHorizontalAlignment((TextHAlignment)halign);
}


float UILabel::getFontSize() const {
	return m_pLabel->getTTFConfig().fontSize;
}

int UILabel::getLineCount() const {
	return m_pLabel->getStringNumLines();
}

bool UILabel::init() {

	setInitialUISize(m_pInfo->Size);

	m_pLabel = SGLabel::createWithTTF(m_pInfo->Text.ToStd(), getFontPath().Source(), m_pInfo->FontSize * CoreClientInfo_v->UIScaleYFactor, Size::ZERO);
	m_pLabel->setHorizontalAlignment((TextHAlignment)m_pInfo->TextHAlignment);
	m_pLabel->setVerticalAlignment((TextVAlignment)m_pInfo->TextVAlignment);
	m_pLabel->setDimensions(m_UISize.width, m_UISize.height);
	m_pLabel->setTextColor(m_pInfo->FontColor);
	m_pLabel->enableWrap(m_pInfo->TextWrap);
	m_pLabel->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pLabel);

	
	return m_bInitialized = true;
}


