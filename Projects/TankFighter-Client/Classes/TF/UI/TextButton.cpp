#include <TF/UI/TextButton.h>

TextButton* TextButton::create(float width, float height, const std::string& content, float fontSize) {
	TextButton* sprite = new TextButton();

	if (sprite->init() && sprite->initWithFile(RECT_IMG_FILENAME) && sprite->init2(width, height, content, fontSize)) {
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

	this->setContentSize({ width, height });

	const Size btnSize = this->getContentSize();

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
	return true;
}


void TextButton::onMouseMove(EventMouse* mouseEvent) {
	if (this->getBoundingBox().containsPoint(mouseEvent->getLocationInView())) {
		if (m_bMouseOver == false) {
			m_bMouseOver = true;
			this->setOpacity(170);
		}
		return;
	}

	if (m_bMouseOver) {
		m_bMouseOver = false;
		this->setOpacity(255);
	}
}



bool TextButton::onTouchBegin(Touch* touch, Event* touchEvent) {
	if (this->getBoundingBox().containsPoint(touch->getLocation())) {
		this->setScale(1.1f);
		if (m_ClickEvent != nullptr)
			this->m_ClickEvent(this);
		m_bMousePressed = true;
	}

	return true;
}

void TextButton::onTouchEnded(Touch* touch, Event* touchEvent) {
	this->setScale(1.0f);
	m_bMouseOver = false;
	m_bMousePressed = false;
}