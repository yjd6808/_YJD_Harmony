/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 5:54:04 PM
 * =====================
 *
 */



#include "Tutturu.h"
#include "SGUILabel.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/SGFontPackage.h>

#include "SGClientInfo.h"

USING_NS_CC;
USING_NS_JC;

SGUILabel::SGUILabel(SGUIGroup* parent, SGUILabelInfo* btnInfo)
	: SGUIElement(parent)
	, m_pInfo(btnInfo)
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

	SGString fontName = SGFontPackage::get()->getFontName(m_pInfo->FontCode);
	SGString fontPath = Path::Combine(CoreCommon_v->DataPath, FontDirName_v, fontName);

	m_pLabel = SGLabel::createWithTTF(m_pInfo->Text.ToStd(), fontPath.Source(), m_pInfo->FontSize, Size::ZERO);
	m_pLabel->setHorizontalAlignment((TextHAlignment)m_pInfo->HorizontalAlignment);
	m_pLabel->setVerticalAlignment((TextVAlignment)m_pInfo->VerticalAlignment);
	m_pLabel->setDimensions(m_pInfo->Width, m_pInfo->Height);
	m_pLabel->enableWrap(m_pInfo->TextWrap);
	m_pLabel->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pLabel);

	return true;
}

int SGUILabel::getCode() {
	return m_pInfo->Code;
}

