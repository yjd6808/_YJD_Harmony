/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 5:54:04 PM
 * =====================
 *
 */



#include "Tutturu.h"
#include "SGUILabel.h"
#include "GameCoreHeader.h"

USING_NS_CC;
USING_NS_JC;

SGUILabel::SGUILabel(SGUIGroup* parent, SGUILabelInfo* labelInfo)
	: SGUIElement(parent, labelInfo)
	, m_pInfo(labelInfo)
	, m_pLabel{nullptr}
{}

SGUILabel::~SGUILabel() {
}

SGUILabel* SGUILabel::create(SGUIGroup* parent, SGUILabelInfo* labelInfo) {
	SGUILabel* pLabel = dbg_new SGUILabel(parent, labelInfo);
	pLabel->init();
	pLabel->autorelease();
	return pLabel;
}

bool SGUILabel::init() {

	const SGString fontName = CoreFont_v->getFontName(m_pInfo->FontCode);
	const SGString fontPath = Path::Combine(CoreCommon_v->DataPath, FontDirName_v, fontName);

	m_pLabel = SGLabel::createWithTTF(m_pInfo->Text.ToStd(), fontPath.Source(), m_pInfo->FontSize, Size::ZERO);
	m_pLabel->setHorizontalAlignment((TextHAlignment)m_pInfo->TextHAlignment);
	m_pLabel->setVerticalAlignment((TextVAlignment)m_pInfo->TextVAlignment);
	m_pLabel->setDimensions(m_pInfo->Size.width, m_pInfo->Size.height);
	m_pLabel->setTextColor(m_pInfo->FontColor);
	m_pLabel->enableWrap(m_pInfo->TextWrap);
	m_pLabel->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pLabel);

	return true;
}

int SGUILabel::getCode() {
	return m_pInfo->Code;
}


