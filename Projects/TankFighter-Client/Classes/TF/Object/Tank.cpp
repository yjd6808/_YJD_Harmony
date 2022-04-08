#include <TF/Object/Tank.h>

Tank* Tank::create(int characterUID, Layer* activeLayer) {
	Tank* sprite = new Tank(characterUID, activeLayer);

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


	// ��ũ ���� - ���� �߻�밡 �����Ǵ� �κ�
	m_pTower = Scale9Sprite::create(RECT_IMG_FILENAME);
	m_pTower->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pTower->setContentSize({ TANK_WIDTH * 2.0 / 3.0f, TANK_WIDTH * 2.0 / 3.0f }); // �ʺ��� 3���� 2
	m_pTower->setColor(Color3B::ORANGE);
	m_pTower->setPosition(this->getContentSize() / 2);
	this->addChild(m_pTower);

	// ��ũ ���� - ��ũ ���뿡 �����Ǵ� �κ�
	m_pGun = Scale9Sprite::create(RECT_IMG_FILENAME);
	m_pGun->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	m_pGun->setContentSize({ TANK_WIDTH / 6.0f, TANK_WIDTH });	// ���� �ʺ�� ��ũ �ʺ��� 6���� 1, ���̴� ��ũ �ʺ�
	m_pGun->setPosition(m_pTower->getContentSize().width / 2, m_pTower->getContentSize().height);
	m_pTower->addChild(m_pGun);

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Tank::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Tank::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);


	return true;
}


void Tank::updatePosition(float delta) {
	if (m_iCharacterUID == _Client->GetCharacterUID()) {
		if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
			m_TankMove.MoveDir = static_cast<Int8>(MoveDirection::Forward);
		} else if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
			m_TankMove.MoveDir = static_cast<Int8>(MoveDirection::Backward);
		} else {
			m_TankMove.MoveDir = static_cast<Int8>(MoveDirection::None);
		}
	}

	m_PrevPos = this->getPosition();;
	auto nextPos = this->getPosition();
	const float curRotation = this->getRotation();

	const MoveDirection dir = static_cast<MoveDirection>(m_TankMove.MoveDir);
	switch (dir) {
	case MoveDirection::Forward:
		nextPos.x += m_TankMove.MoveSpeed * sinf(CC_DEGREES_TO_RADIANS(curRotation)) * delta;
		nextPos.y += m_TankMove.MoveSpeed * cosf(CC_DEGREES_TO_RADIANS(curRotation)) * delta;
		break;
	case MoveDirection::Backward:
		nextPos.x -= m_TankMove.MoveSpeed * sinf(CC_DEGREES_TO_RADIANS(curRotation)) * delta;
		nextPos.y -= m_TankMove.MoveSpeed * cosf(CC_DEGREES_TO_RADIANS(curRotation)) * delta;
		break;
	}

	this->setPosition(nextPos);
}

void Tank::updateRotation(float delta) {
	if (m_iCharacterUID == _Client->GetCharacterUID()) {
		if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
			m_TankMove.RotationDir = static_cast<Int8>(RotateDirection::Left);
		} else if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
			m_TankMove.RotationDir = static_cast<Int8>(RotateDirection::Right);
		} else {
			m_TankMove.RotationDir = static_cast<Int8>(RotateDirection::None);
		}
	}

	m_fPrevRot = this->getRotation();
	float fNextRot = this->getRotation();
	const RotateDirection dir = static_cast<RotateDirection>(m_TankMove.RotationDir);
	switch (dir) {
	case RotateDirection::Left:
		fNextRot -= m_TankMove.RotationSpeed * delta;
		break;
	case RotateDirection::Right:
		fNextRot += m_TankMove.RotationSpeed * delta;
		break;
	}

	this->setRotation(fNextRot);
}

void Tank::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {

	// �ڱ��ڽ��� ��ũ�� �� �� �ֵ��� �Ѵ�.
	if (m_iCharacterUID == _Client->GetCharacterUID()) {
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
			if (m_bMoveable) {
				m_KeyPressedMap.Insert(keyCode, true);
			}
			break;

		case EventKeyboard::KeyCode::KEY_SPACE:
			if (m_bFireable) {
				m_KeyPressedMap.Insert(keyCode, true);
			}
			break;
		}
		
	}
}

void Tank::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {

	// �ڱ��ڽ��� ��ũ�� �� �� �ֵ��� �Ѵ�.
	if (m_iCharacterUID == _Client->GetCharacterUID() && m_KeyPressedMap.Exist(keyCode))
		m_KeyPressedMap.Remove(keyCode);
}

void Tank::UpdateTankMove(TankMove& move) {
	Memory::CopyUnsafe(&m_TankMove, &move, sizeof(TankMove));

	this->setPosition(m_TankMove.X, m_TankMove.Y);
	this->setRotation(m_TankMove.Rotation);
	m_bUpdated = true;
}
