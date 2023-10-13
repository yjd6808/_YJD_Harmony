/*
	생성일 : 2019/03/05
	작성자 : 윤정도
	옛날에 만든거 가져옴
	이게 쓸일이 생기네
 */


#include "Pch.h"
#include "GridLayer.h"

USING_NS_CC;

GridLayer::GridLayer(const int interval, const GridEvent userGridEvent) {
	this->interval = interval;
	this->userGridEvent = userGridEvent;
	m_pDrawNode = nullptr;
}

GridLayer::~GridLayer() {
}

GridLayer * GridLayer::create(const int interval, const Color4F & color, const GridEvent userGridEvent)
{
	GridLayer *layer = dbg_new GridLayer(interval, userGridEvent);
	if (layer && layer->initWithParams(interval, color))
	{
		layer->autorelease();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return nullptr;
}


void GridLayer::DrawGridWindow(const int interval, const Color4F& color) {
	if (m_pDrawNode) {
		return;
	}

	const int WIN_WIDTH = Director::getInstance()->getWinSize().width;
	const int WIN_HEIGHT = Director::getInstance()->getWinSize().height;

	struct DrawNodeFactory
	{
		static DrawNode* create() {
			DrawNode* pNode = dbg_new DrawNode;
			pNode->init();
			pNode->autorelease();
			return pNode;
		}
	};

	m_pDrawNode = DrawNodeFactory::create();
	for (int i = -10'000; i < 10'000; i += interval)
		m_pDrawNode->drawLine(Vec2(0, i), Vec2(WIN_WIDTH, i), color);
	for (int i = -10'000; i < 10'000; i += interval)
		m_pDrawNode->drawLine(Vec2(i, 0), Vec2(i, WIN_HEIGHT), color);
	
	this->addChild(m_pDrawNode, 0);
}

bool GridLayer::initWithParams(const int interval, const Color4F & color)
{
	if (!Layer::init())
		return false;

	float fScaleFactor = Director::getInstance()->getContentScaleFactor();

	if (userGridEvent == GridEvent::ShowGridAndMousePoint || userGridEvent == GridEvent::ShowGrid)
	{
		DrawGridWindow(interval, color);
	}

	if (userGridEvent == GridEvent::ShowGridAndMousePoint || userGridEvent == GridEvent::ShowMousePoint)
	{
		EventListenerMouse* listner = EventListenerMouse::create();
		listner->onMouseMove = CC_CALLBACK_1(GridLayer::onMouseMove, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listner, this);

		EventListenerTouchOneByOne* listnerTouch = EventListenerTouchOneByOne::create();
		listnerTouch->onTouchBegan = CC_CALLBACK_2(GridLayer::onTouchBegan, this);
		listnerTouch->onTouchEnded = CC_CALLBACK_2(GridLayer::onTouchEnded, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listnerTouch, this);

		Label* GridMousePos = Label::create();
		GridMousePos->setPosition(Vec2(50.0f, Director::getInstance()->getWinSize().height / fScaleFactor - 50.0f));
		GridMousePos->setAnchorPoint(Vec2::ZERO);
		GridMousePos->setSystemFontSize(20);
		this->addChild(GridMousePos, 1, (int)LabelTag::GridMousePosition);

		Label* MouseMovePos = Label::create();
		MouseMovePos->setPosition(Vec2(50.0f, Director::getInstance()->getWinSize().height / fScaleFactor - 70.0f));
		MouseMovePos->setAnchorPoint(Vec2::ZERO);
		MouseMovePos->setSystemFontSize(20);
		this->addChild(MouseMovePos, 1, (int)LabelTag::MouseMovePosition);

		Label* TouchMousePos = Label::create();
		TouchMousePos->setPosition(Vec2(50.0f, Director::getInstance()->getWinSize().height / fScaleFactor - 90.0f));
		TouchMousePos->setAnchorPoint(Vec2::ZERO);
		TouchMousePos->setSystemFontSize(20);
		this->addChild(TouchMousePos, 1, (int)LabelTag::TouchMousePosition);

		
	}

	return true;
}

bool GridLayer::onMouseMove(cocos2d::Event* event)
{
	if (userGridEvent == GridEvent::ShowGridAndMousePoint || userGridEvent == GridEvent::ShowMousePoint)
	{
		EventMouse* e = (EventMouse*)event;
		Label* label1 = (Label*)this->getChildByTag((int)LabelTag::GridMousePosition);
		Label* label2 = (Label*)this->getChildByTag((int)LabelTag::MouseMovePosition);

		int Xpos = (int)e->getCursorX();
		int Ypos = (int)e->getCursorY();

		int ConvertedXpos = 0;
		int ConvertedYpos = 0;

		int PercentDividerX = Xpos % this->interval;
		int PercentDividerY = Ypos % this->interval;

		if (PercentDividerX > this->interval / 2) ConvertedXpos = Xpos + this->interval - PercentDividerX;
		else ConvertedXpos = Xpos - PercentDividerX;

		if (PercentDividerY > this->interval / 2) ConvertedYpos = Ypos + this->interval - PercentDividerY;
		else ConvertedYpos = Ypos - PercentDividerY;

		label1->setString(StringUtils::format("%d / %d", ConvertedXpos, ConvertedYpos));
		label2->setString(StringUtils::format("%d / %d", Xpos, Ypos));
	}
	return true;
}

bool GridLayer::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event *)
{
	Label* label = (Label*)this->getChildByTag((int)LabelTag::TouchMousePosition);
	label->setVisible(true);
	label->setString(StringUtils::format("%d / %d", (int)touch->getLocation().x, (int)touch->getLocation().y));

	return true;
}

void GridLayer::onTouchEnded(cocos2d::Touch *, cocos2d::Event *)
{
	Label* label = (Label*)this->getChildByTag((int)LabelTag::TouchMousePosition);
	label->setVisible(false);

}

void GridLayer::onExit() {
	Layer::onExit();
}
