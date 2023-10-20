/*
 * 작성자 : 윤정도
 */

#include "Pch.h"
#include "ColoredListView.h"

USING_NS_CC;
USING_NS_CCUI;

ColoredListView * ColoredListView::create(const Color4B& color)
{
	ColoredListView *ret = new (std::nothrow) ColoredListView();
	if (ret &&ret->init() && ret->initWithColor(color))
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

bool ColoredListView::init()
{
	if (!Layer::init())
		return false;

	m_pListView = ListView::create();
	m_pListView->setDirection(ScrollView::Direction::VERTICAL);
	m_pListView->setScrollBarEnabled(true);
	m_pListView->setScrollBarAutoHideEnabled(false);
	m_pListView->setAnchorPoint(Vec2::ZERO);
	m_pListView->addEventListener(CC_CALLBACK_2(ColoredListView::OnScrollViewEvent, this));
	this->addChild(m_pListView);

	return true;
}

void ColoredListView::SetContantSize(const Size& size) {
	this->setContentSize(size);

	// ListView 컨텐츠 사이즈를 강제로 변경하면 경고뜸
	// "Inner height <= scrollview height, it will be force sized!" 경고 뜸 일단 주석처리함.
	m_pListView->setContentSize(size);
}

void ColoredListView::OnScrollViewEvent(Ref* pref, ScrollView::EventType type) {
}
