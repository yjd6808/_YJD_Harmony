/*
	생성일 : 2019/03/05
	작성자 : 윤정도
	옛날에 만든거 가져옴
	이게 쓸일이 생기네
 */

#include "Core.h"
#include "GridLayer.h"

USING_NS_CC;

//////////////////////////////////////////////////////////////////////////////////////////
GridLayer::GridLayer(const int _interval, const GridEvent _userGridEvent)
{
	interval_ = _interval;
	userGridEvent_ = _userGridEvent;
	drawNode_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
GridLayer::~GridLayer()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
GridLayer* GridLayer::create(const int _interval, const Color4F& _color, const GridEvent _userGridEvent)
{
	GridLayer* pLayer = dbg_new GridLayer(_interval, _userGridEvent);
	if (pLayer && pLayer->InitWithParams(_interval, _color))
	{
		pLayer->autorelease();
		return pLayer;
	}
	CC_SAFE_DELETE(pLayer);
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GridLayer::DrawGridWindow(const int _interval, const Color4F& _color)
{
	if (drawNode_)
		return;

	const int WIN_WIDTH = Director::getInstance()->getWinSize().width;
	const int WIN_HEIGHT = Director::getInstance()->getWinSize().height;

	struct DrawNodeFactory
	{
		static DrawNode* create()
		{
			DrawNode* pNode = dbg_new DrawNode;
			pNode->init();
			pNode->autorelease();
			return pNode;
		}
	};

	drawNode_ = DrawNodeFactory::create();
	for (int i = -10'000; i < 10'000; i += _interval)
		drawNode_->drawLine(Vec2(0, i), Vec2(WIN_WIDTH, i), _color);
	for (int i = -10'000; i < 10'000; i += _interval)
		drawNode_->drawLine(Vec2(i, 0), Vec2(i, WIN_HEIGHT), _color);

	addChild(drawNode_, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GridLayer::InitWithParams(const int _interval, const Color4F& _color)
{
	if (!Layer::init())
		return false;

	float scaleFactor = Director::getInstance()->getContentScaleFactor();

	if (userGridEvent_ == GridEvent::ShowGridAndMousePoint || userGridEvent_ == GridEvent::ShowGrid)
	{
		DrawGridWindow(_interval, _color);
	}

	if (userGridEvent_ == GridEvent::ShowGridAndMousePoint || userGridEvent_ == GridEvent::ShowMousePoint)
	{
		EventListenerMouse* pListenerMouse = EventListenerMouse::create();
		pListenerMouse->onMouseMove = CC_CALLBACK_1(GridLayer::OnMouseMove, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(pListenerMouse, this);

		EventListenerTouchOneByOne* pListenerTouch = EventListenerTouchOneByOne::create();
		pListenerTouch->onTouchBegan = CC_CALLBACK_2(GridLayer::onTouchBegan, this);
		pListenerTouch->onTouchEnded = CC_CALLBACK_2(GridLayer::onTouchEnded, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(pListenerTouch, this);

		Label* pGridMousePositionLabel = Label::create();
		pGridMousePositionLabel->setPosition(Vec2(50.0f, Director::getInstance()->getWinSize().height / scaleFactor - 50.0f));
		pGridMousePositionLabel->setAnchorPoint(Vec2::ZERO);
		pGridMousePositionLabel->setSystemFontSize(20);
		addChild(pGridMousePositionLabel, 1, static_cast<int>(LabelTag::GridMousePosition));

		Label* pMouseMovePositionLabel = Label::create();
		pMouseMovePositionLabel->setPosition(Vec2(50.0f, Director::getInstance()->getWinSize().height / scaleFactor - 70.0f));
		pMouseMovePositionLabel->setAnchorPoint(Vec2::ZERO);
		pMouseMovePositionLabel->setSystemFontSize(20);
		addChild(pMouseMovePositionLabel, 1, static_cast<int>(LabelTag::MouseMovePosition));

		Label* pTouchMousePositionLabel = Label::create();
		pTouchMousePositionLabel->setPosition(Vec2(50.0f, Director::getInstance()->getWinSize().height / scaleFactor - 90.0f));
		pTouchMousePositionLabel->setAnchorPoint(Vec2::ZERO);
		pTouchMousePositionLabel->setSystemFontSize(20);
		addChild(pTouchMousePositionLabel, 1, static_cast<int>(LabelTag::TouchMousePosition));
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GridLayer::OnMouseMove(cocos2d::Event* _pEvent)
{
	if (userGridEvent_ == GridEvent::ShowGridAndMousePoint || userGridEvent_ == GridEvent::ShowMousePoint)
	{
		EventMouse* pEventMouse = static_cast<EventMouse*>(_pEvent);
		Label* pGridMousePositionLabel = static_cast<Label*>(getChildByTag(static_cast<int>(LabelTag::GridMousePosition)));
		Label* pMouseMovePositionLabel = static_cast<Label*>(getChildByTag(static_cast<int>(LabelTag::MouseMovePosition)));

		int xPos = static_cast<int>(pEventMouse->getCursorX());
		int yPos = static_cast<int>(pEventMouse->getCursorY());

		int convertedXPos = 0;
		int convertedYPos = 0;

		int percentDividerX = xPos % interval_;
		int percentDividerY = yPos % interval_;

		if (percentDividerX > interval_ / 2)
			convertedXPos = xPos + interval_ - percentDividerX;
		else
			convertedXPos = xPos - percentDividerX;

		if (percentDividerY > interval_ / 2)
			convertedYPos = yPos + interval_ - percentDividerY;
		else
			convertedYPos = yPos - percentDividerY;

		pGridMousePositionLabel->setString(StringUtils::format("%d / %d", convertedXPos, convertedYPos));
		pMouseMovePositionLabel->setString(StringUtils::format("%d / %d", xPos, yPos));
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GridLayer::onTouchBegan(cocos2d::Touch* _pTouch, cocos2d::Event* _pEvent)
{
	Label* pTouchMousePositionLabel = static_cast<Label*>(getChildByTag(static_cast<int>(LabelTag::TouchMousePosition)));
	pTouchMousePositionLabel->setVisible(true);
	pTouchMousePositionLabel->setString(StringUtils::format("%d / %d", static_cast<int>(_pTouch->getLocation().x), static_cast<int>(_pTouch->getLocation().y)));

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GridLayer::onTouchEnded(cocos2d::Touch* _pTouch, cocos2d::Event* _pEvent)
{
	Label* pTouchMousePositionLabel = static_cast<Label*>(getChildByTag(static_cast<int>(LabelTag::TouchMousePosition)));
	pTouchMousePositionLabel->setVisible(false);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GridLayer::onExit()
{
	Layer::onExit();
}
