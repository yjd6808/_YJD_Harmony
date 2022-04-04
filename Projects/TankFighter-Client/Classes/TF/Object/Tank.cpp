#include <TF/Object/Tank.h>

Tank* Tank::create() {
	Tank* sprite = new Tank();

	if (sprite->init() && sprite->initWithFile(RECT_IMG_FILENAME) && sprite->init2()) {
		sprite->autorelease();
		return sprite;
	}

	CC_SAFE_DELETE(sprite);
	return nullptr;
}

bool Tank::init2() {

	this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	this->setContentSize({ TANK_WIDTH, TANK_HEIGHT });
	this->setColor(Color3B::GREEN);

	for (int i = 0; i < 2; i++) {
		m_pBodyCollidors[i] = Sprite::create(CIRCLE_IMG_FILENAME);
		m_pBodyCollidors[i]->setContentSize({ TANK_WIDTH, TANK_WIDTH });
		m_pBodyCollidors[i]->setPositionX(TANK_WIDTH / 2.0f);
		m_pBodyCollidors[i]->setVisible(true);
		if (i == 0)
			m_pBodyCollidors[i]->setPositionY(TANK_WIDTH / 2.0f);
		else
			m_pBodyCollidors[i]->setPositionY(TANK_WIDTH / 2.0f + TANK_WIDTH);
		this->addChild(m_pBodyCollidors[i]);
	}


	// 탱크 포대 - 대포 발사대가 장착되는 부분
	m_pTower = Scale9Sprite::create(RECT_IMG_FILENAME);
	m_pTower->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pTower->setContentSize({ TANK_WIDTH * 2.0 / 3.0f, TANK_WIDTH * 2.0 / 3.0f }); // 너비의 3분의 2
	m_pTower->setColor(Color3B::ORANGE);
	m_pTower->setPosition(this->getContentSize() / 2);
	this->addChild(m_pTower);

	// 탱크 포신 - 탱크 포대에 장착되는 부분
	m_pGun = Scale9Sprite::create(RECT_IMG_FILENAME);
	m_pGun->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	m_pGun->setContentSize({ TANK_WIDTH / 6.0f, TANK_WIDTH });	// 포신 너비는 탱크 너비의 6분의 1, 높이는 탱크 너비
	m_pGun->setPosition(m_pTower->getContentSize().width / 2, m_pTower->getContentSize().height);
	m_pTower->addChild(m_pGun);

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Tank::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Tank::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);


	return true;
}


void Tank::updatePosition(float delta) {
	if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
		m_eMoveDir = MoveDirection::Forward;
	} else if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
		m_eMoveDir = MoveDirection::Backward;
	} else {
		m_eMoveDir = MoveDirection::None;
	}

	m_PrevPos = this->getPosition();;
	auto nextPos = this->getPosition();
	float curRotation = this->getRotation();
	// CCLOG("%f", sinf(CC_DEGREES_TO_RADIANS(m_fCurRotation)));
	switch (m_eMoveDir) {
	case MoveDirection::Forward:
		nextPos.x += m_fTankMoveSpeed * sinf(CC_DEGREES_TO_RADIANS(curRotation)) * delta;
		nextPos.y += m_fTankMoveSpeed * cosf(CC_DEGREES_TO_RADIANS(curRotation)) * delta;
		break;
	case MoveDirection::Backward:
		nextPos.x -= m_fTankMoveSpeed * sinf(CC_DEGREES_TO_RADIANS(curRotation)) * delta;
		nextPos.y -= m_fTankMoveSpeed * cosf(CC_DEGREES_TO_RADIANS(curRotation)) * delta;
		break;
	}

	this->setPosition(nextPos);
}

void Tank::updateRotation(float delta) {
	if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
		m_eRotateDir = RotateDirection::Left;
	} else if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
		m_eRotateDir = RotateDirection::Right;
	} else {
		m_eRotateDir = RotateDirection::None;
	}

	m_fPrevRot = this->getRotation();
	float fNextRot = this->getRotation();
	switch (m_eRotateDir) {
	case RotateDirection::Left:
		fNextRot -= m_fTankRotationSpeed * delta;
		break;
	case RotateDirection::Right:
		fNextRot += m_fTankRotationSpeed * delta;
		break;
	}

	this->setRotation(fNextRot);
}

void Tank::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
	m_KeyPressedMap.Insert(keyCode, true);
}

void Tank::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
	if (m_KeyPressedMap.Exist(keyCode))
		m_KeyPressedMap.Remove(keyCode);
}
