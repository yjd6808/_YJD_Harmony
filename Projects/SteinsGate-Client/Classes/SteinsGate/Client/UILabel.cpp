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
	m_pLabel->initWithTTF(text, getFontPath().Source(), m_pInfo->FontSize, m_pInfo->Size);
}

void UILabel::setText(const std::string& text, float width, float height) {
	m_pLabel->initWithTTF(text, getFontPath().Source(), m_pInfo->FontSize, SGSize{ width, height });
}

void UILabel::setContentSize(const SGSize& contentSize) {
	if (!m_bResizable)
		return;

	Size prevSize = _contentSize;
	UIElement::setContentSize(contentSize);

	if (m_pLabel == nullptr)
		return;

	m_pLabel->setDimensions(_contentSize.width, _contentSize.height);

	float fScaleX = _contentSize.width / prevSize.width;
	float fScaleY = _contentSize.height / prevSize.height;

	// TODO: 폰트 사이즈도 변경되야함.
}

bool UILabel::init() {

	m_pLabel = SGLabel::createWithTTF(m_pInfo->Text.ToStd(), getFontPath().Source(), m_pInfo->FontSize, Size::ZERO);
	m_pLabel->setHorizontalAlignment((TextHAlignment)m_pInfo->TextHAlignment);
	m_pLabel->setVerticalAlignment((TextVAlignment)m_pInfo->TextVAlignment);
	m_pLabel->setDimensions(m_pInfo->Size.width, m_pInfo->Size.height);
	m_pLabel->setTextColor(m_pInfo->FontColor);
	m_pLabel->enableWrap(m_pInfo->TextWrap);
	m_pLabel->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pLabel);

	_contentSize = m_pInfo->Size;
	return true;
}


