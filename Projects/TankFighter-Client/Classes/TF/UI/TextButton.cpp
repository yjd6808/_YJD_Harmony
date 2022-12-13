/*
 * 작성자 : 윤정도
 */

#include <TF/UI/TextButton.h>

TextButton* TextButton::create(float width, float height, const std::string& content, float fontSize) {
	TextButton* sprite = new TextButton();

	if (sprite->init() && /*sprite->initWithFile(RECT_IMG_FILENAME) &&*/ sprite->init2(width, height, content, fontSize)) {
		sprite->autorelease();
		return sprite;
	}

	CC_SAFE_DELETE(sprite);
	return nullptr;
}

void TextButton::setFontColor(const Color3B& color ) {
	m_pTextUI->setColor(color);
}



bool TextButton::init2(float width, float height, const std::string& content, float fontSize) {

	this->setScale9Enabled(true);
	this->setSize({ width, height });
	this->setContentSize({ width, height });

	loadTextures(RECT_IMG_FILENAME, "", "", TextureResType::LOCAL);


	const Size btnSize = this->getContentSize();

	m_pBackground = Scale9Sprite::create(RECT_IMG_FILENAME);
	m_pBackground->setScale9Enabled(true);
	m_pBackground->setAnchorPoint(Vec2::ZERO);
	m_pBackground->setContentSize(btnSize);
	m_pBackground->setPosition(0, 0);
	this->addChild(m_pBackground);

	m_pTextUI = Text::create(content, FONT_PATH_DNF, fontSize);
	m_pTextUI->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pTextUI->setPosition({ btnSize.width / 2, btnSize.height / 2 });
	m_pTextUI->setColor(Color3B::BLACK);
	this->addChild(m_pTextUI);

	EventListenerTouchOneByOne* listner = EventListenerTouchOneByOne::create();
	listner->onTouchBegan = CC_CALLBACK_2(TextButton::onTouchBegin, this);
	listner->onTouchEnded = CC_CALLBACK_2(TextButton::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listner, this);

	EventListenerMouse* mouseLietener = EventListenerMouse::create();
	mouseLietener->onMouseMove = CC_CALLBACK_1(TextButton::onMouseMove, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseLietener, this);

	// 버튼의 기능은 이녀석이 수행
	Button* m_pButton = Button::create(TRANSPARENT_RECT_IMG_FILENAME);
	m_pButton->setAnchorPoint(Vec2::ZERO);
	m_pButton->setSize(btnSize);
	m_pButton->setScale9Enabled(true);
	m_pButton->setContentSize(btnSize);
	m_pButton->addClickEventListener(CC_CALLBACK_1(TextButton::onMouseClick, this));
	this->addChild(m_pButton);

	return true;

}


void TextButton::onMouseMove(EventMouse* mouseEvent) {
	Rect thisRect = this->getBoundingBox();
	thisRect.origin = this->getParent()->convertToWorldSpace(thisRect.origin);
	if (thisRect.containsPoint(mouseEvent->getLocationInView())) {
		if (m_bMouseOver == false) {
			m_bMouseOver = true;
			m_pBackground->setOpacity(170);
		}
		return;
	}

	if (m_bMouseOver) {
		m_bMouseOver = false;
		m_pBackground->setOpacity(255);
	}
}



bool TextButton::onTouchBegin(Touch* touch, cocos2d::Event* touchEvent) {
	return true;
	Rect thisRect = this->getBoundingBox();
	thisRect.origin = this->getPosition();
	//thisRect.origin = this->getParent()->convertToWorldSpace(thisRect.origin);	// Perfect Location 얻기
	Vec2 touchPoint = touch->getStartLocation();
	if (thisRect.containsPoint(touchPoint)) {
		m_pBackground->setScale(1.1f);
		m_bMousePressed = true;
	}

	return true;
}

void TextButton::onTouchEnded(Touch* touch, cocos2d::Event* touchEvent) {
	m_pBackground->setScale(1.0f);
	m_bMouseOver = false;
	m_bMousePressed = false;
}

void TextButton::setBackgroundColor(const Color3B& color) {
	m_pBackground->setColor(color);
}

void TextButton::onMouseClick(Ref* ref) {
	if (m_ClickEvent)
		m_ClickEvent(this);
}
