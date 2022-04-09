#include <TF/Object/Tank.h>
#include <JCore/Random.h>
#include <TF/Util/ColorUtil.h>

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
		m_pBodyCollidors[i]->setVisible(false);
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
	m_pGun->setContentSize({ TANK_WIDTH / 6.0f, TANK_WIDTH });	// 포신 너비는 탱크 너비의 6분의 1, 높이는 탱크 세로길이
	m_pGun->setPosition(m_pTower->getContentSize().width / 2, m_pTower->getContentSize().height);
	m_pTower->addChild(m_pGun);

	m_pFirePos = Node::create();
	m_pFirePos->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pFirePos->setPosition({ 15.0f , 75.0f});
	this->addChild(m_pFirePos);

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Tank::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Tank::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);




	return true;
}


void Tank::updatePosition(float delta) {
	if (GetCharacterUID() == _Client->GetCharacterUID()) {
		if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
			m_TankMove.MoveDir = MoveDirection::Forward;
		} else if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
			m_TankMove.MoveDir = MoveDirection::Backward;
		} else {
			m_TankMove.MoveDir = MoveDirection::None;
		}
	}

	auto nextPos = this->getPosition();
	const float curRotation = this->getRotation();

	const MoveDirection dir = m_TankMove.MoveDir;
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
	m_TankMove.X = nextPos.x;
	m_TankMove.Y = nextPos.y;
}

void Tank::updateShotDelay(float delta) {
	m_fShotDelay += delta;

	if (m_fShotDelay >= TANK_FIRE_DELAY) {
		m_bShotable = true;
	}
}

void Tank::updateRotation(float delta) {
	if (GetCharacterUID() == _Client->GetCharacterUID()) {
		if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
			m_TankMove.RotationDir = RotateDirection::Left;
		} else if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
			m_TankMove.RotationDir = RotateDirection::Right;
		} else {
			m_TankMove.RotationDir = RotateDirection::None;
		}
	}

	float fNextRot = this->getRotation();
	const RotateDirection dir = m_TankMove.RotationDir;
	switch (dir) {
	case RotateDirection::Left:
		fNextRot -= m_TankMove.RotationSpeed * delta;
		break;
	case RotateDirection::Right:
		fNextRot += m_TankMove.RotationSpeed * delta;
		break;
	}

	this->setRotation(fNextRot);
	m_TankMove.Rotation = fNextRot;
}



void Tank::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {

	// 자기자신의 탱크만 몰 수 있도록 한다.
	if (GetCharacterUID() == _Client->GetCharacterUID()) {
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
			if (m_bFireable && m_bShotable) {
				Fire();

				m_fShotDelay = 0.0f;
				m_bShotable = false;
			}
			break;
		}
		
	}
}

void Tank::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {

	// 자기자신의 탱크만 몰 수 있도록 한다.
	if (GetCharacterUID() == _Client->GetCharacterUID() && m_KeyPressedMap.Exist(keyCode))
		m_KeyPressedMap.Remove(keyCode);
}

void Tank::UpdateTankMove(TankMove& move) {
	Memory::CopyUnsafe(&m_TankMove, &move, sizeof(TankMove));

	this->setPosition(m_TankMove.X, m_TankMove.Y);
	this->setRotation(m_TankMove.Rotation);
	m_bUpdated = true;
}

void Tank::Fire() {
	Vec2 firePos = this->convertToWorldSpace(m_pFirePos->getPosition());

	Bullet* pBullet = Bullet::create(10.0f, _Client->GetCharacterUID());
	pBullet->setPosition(firePos);
	pBullet->setRotation(this->getRotation());
	pBullet->setColor(ColorList::Africanviolet_v);
	m_pActiveLayer->addChild(pBullet);

	if (m_FilreCallBack)
		m_FilreCallBack(pBullet);
}
