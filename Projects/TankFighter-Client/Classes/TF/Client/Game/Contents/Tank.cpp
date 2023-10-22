/*
 * 작성자: 윤정도
 * 생성일: 10/20/2023 5:52:31 PM
 * =====================
 *
 */

#include "Pch.h"
#include "Tank.h"

#include <TF/Common/Command.h>
#include <TF/Client/Net/Send/S_GAME.h>

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;


Tank::Tank()
	: m_pBodyCollidors{}
    , m_pTower(nullptr)
    , m_pGun(nullptr)
    , m_pFirePosition(nullptr)
    , m_bMoveable(false)
    , m_bFireable(false)
    , m_Move()
	, m_pInfo(nullptr)
	, m_fShotTimeCounter(TimeCounterAttribute::TimeOverReset)
{}

bool Tank::init() {

	if (!Scale9Sprite::init() || !initWithFile(Const::Resource::WhiteRectFileName)) {
		return false;
	}

	const float iBodyWidth = Const::Tank::BodyWidth;
	const float iBodyHeight = Const::Tank::BodyHeight;

	setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	setContentSize({ iBodyWidth, iBodyHeight });
	setColor(Color3B::GREEN);

	for (int i = 0; i < 2; i++) {
		Sprite* pCollider = Sprite::create(Const::Resource::WhiteCircleFileName);
		pCollider->setContentSize({ iBodyWidth, iBodyWidth });
		pCollider->setPositionX(iBodyWidth / 2.0f);
		pCollider->setVisible(false);
		this->addChild(pCollider);
		m_pBodyCollidors[i] = pCollider;
	}

	// 한개는 아래 한개는 위에
	m_pBodyCollidors[0]->setPositionY(iBodyWidth / 2.0f);
	m_pBodyCollidors[1]->setPositionY(iBodyWidth / 2.0f + iBodyWidth);

	// 탱크 포대 - 대포 발사대가 장착되는 부분
	m_pTower = Sprite::create(Const::Resource::WhiteRectFileName);
	m_pTower->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pTower->setContentSize({ iBodyWidth * 2.0f / 3.0f, iBodyWidth * 2.0f / 3.0f }); // 너비의 3분의 2
	m_pTower->setColor(Color3B::ORANGE);
	m_pTower->setPosition(this->getContentSize() / 2);
	this->addChild(m_pTower);

	// 탱크 포신 - 탱크 포대에 장착되는 부분
	m_pGun = Sprite::create(Const::Resource::WhiteRectFileName);
	m_pGun->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	m_pGun->setContentSize({ iBodyWidth / 6.0f, iBodyWidth });	// 포신 너비는 탱크 너비의 6분의 1, 높이는 탱크 세로길이
	m_pGun->setPosition(m_pTower->getContentSize().width / 2, m_pTower->getContentSize().height);
	m_pTower->addChild(m_pGun);

	m_pFirePosition = Node::create();
	m_pFirePosition->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pFirePosition->setPosition({ 15.0f , 75.0f});
	this->addChild(m_pFirePosition);

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Tank::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Tank::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	return true;
}

void Tank::update(float delta) {
	updateRotation(delta);
	updatePosition(delta);
	updateShotFire(delta);

	static TimeCounterF s_MoveSendCounter(TimeCounterAttribute::TimeOverReset);
	if (s_MoveSendCounter.ElapsedSeconds(0.1f)) {
		sendMove();
	}
}

void Tank::sendMove() {
	if (getCharacterPrimaryKey() != Core::GameClient->GetCharacterPrimaryKey()) {
		return;
	}

	TankMoveNet move;
	getMove(move);
	move.CharacterPrimaryKey = m_pInfo == nullptr ? Const::InvalidValue : m_pInfo->PrimaryKey;
}

void Tank::getMove(JCORE_REF_OUT TankMove& move) {
	move = m_Move;
}

void Tank::updateRotation(float delta) {
	updateRotateDirection();
	
	float fNextRot = this->getRotation();

	switch (m_Move.RotationDir) {
	case RotateDirection::Left:
		fNextRot -= m_Move.RotationSpeed * delta;
		break;
	case RotateDirection::Right:
		fNextRot += m_Move.RotationSpeed * delta;
		break;
	default: ;
	}

	this->setRotation(fNextRot);
	m_Move.Rotation = fNextRot;
}

void Tank::updatePosition(float delta) {
	updateMoveDirection();

	Vec2 nextPos = getPosition();
	const float fCurRotation = this->getRotation();

	switch (m_Move.MoveDir) {
	case MoveDirection::Forward:
		nextPos.x += m_Move.MoveSpeed * sinf(CC_DEGREES_TO_RADIANS(fCurRotation)) * delta;
		nextPos.y += m_Move.MoveSpeed * cosf(CC_DEGREES_TO_RADIANS(fCurRotation)) * delta;
		break;
	case MoveDirection::Backward:
		nextPos.x -= m_Move.MoveSpeed * sinf(CC_DEGREES_TO_RADIANS(fCurRotation)) * delta;
		nextPos.y -= m_Move.MoveSpeed * cosf(CC_DEGREES_TO_RADIANS(fCurRotation)) * delta;
		break;
	default: ;
	}

	setPosition(nextPos);
	m_Move.X = nextPos.x;
	m_Move.Y = nextPos.y;
}

void Tank::updateShotFire(float delta) {
	if (getCharacterPrimaryKey() != Core::GameClient->GetCharacterPrimaryKey()) {
		return;
	}

	if (!m_bFireable)
		return;

	m_fShotTimeCounter.Elapsed += delta;

	if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_SPACE) && m_fShotTimeCounter.ElapsedSeconds(6 / 60.0f)) {
		fire();
	}
}

void Tank::fire() {
	if (m_FilreCallBack)
		m_FilreCallBack(this);
}

void Tank::spawn(RoomCharacterInfo* info, const TankMove& move) {
	m_pInfo = info;
	setTankMove(move);
	setVisible(true);

	// 이하 코드는 주인공 플레이어만 적용
	if (getCharacterPrimaryKey() != Core::GameClient->GetCharacterPrimaryKey())
		return;

	m_bFireable = true;
	m_bMoveable = true;
	m_KeyPressedMap.Clear();
}

void Tank::free() {
	m_pInfo = nullptr;
	m_bFireable = false;
	m_bMoveable = false;
	setVisible(false);
}

void Tank::setTankMove(const TankMove& move) {
	m_Move = move;

	setPosition(m_Move.X, m_Move.Y);
	setRotation(m_Move.Rotation);
}

void Tank::setDeath(bool death) {
	if (m_pInfo == nullptr) {
		DebugAssert(false);
		return;
	}
	m_pInfo->IsDeath = death;
}

int Tank::getCharacterPrimaryKey() const {
	if (m_pInfo == nullptr)
		return Const::InvalidValue;

	return m_pInfo->PrimaryKey;
}

Vec2 Tank::getFirePosition() const {
	return convertToWorldSpace(m_pFirePosition->getPosition());
}

void Tank::updateRotateDirection() {
	if (!m_bMoveable) {
		return;
	}

	if (getCharacterPrimaryKey() != Core::GameClient->GetCharacterPrimaryKey()) {
		return;
	}

	if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
		m_Move.RotationDir = RotateDirection::Left;
	} else if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
		m_Move.RotationDir = RotateDirection::Right;
	} else {
		m_Move.RotationDir = RotateDirection::None;
	}
}



void Tank::updateMoveDirection() {
	if (!m_bMoveable) {
		return;
	}

	if (getCharacterPrimaryKey() != Core::GameClient->GetCharacterPrimaryKey()) {
		return;
	}

	if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
		m_Move.MoveDir = MoveDirection::Forward;
	} else if (m_KeyPressedMap.Exist(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
		m_Move.MoveDir = MoveDirection::Backward;
	} else {
		m_Move.MoveDir = MoveDirection::None;
	}
}


void Tank::onKeyPressed(EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	// 자기자신의 탱크만 몰 수 있도록 한다.
	if (getCharacterPrimaryKey() != Core::GameClient->GetCharacterPrimaryKey()) {
		return;
	}

	m_KeyPressedMap.Insert(keyCode, true);
}

void Tank::onKeyReleased(EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	if (getCharacterPrimaryKey() != Core::GameClient->GetCharacterPrimaryKey()) {
		return;
	}

	m_KeyPressedMap.Remove(keyCode);
}

bool Tank::isCollide(Bullet* bullet) {
	for (int i = 0; i < 2; i++) {
		const Sprite* pCollider = m_pBodyCollidors[i];
		const Vec2 p1 = convertToWorldSpace(pCollider->getPosition());	// Collider World Position
		const Vec2 p2 = bullet->getPosition();							// Bullet World Position    총알은 애초에 최상단 레이어에 AddChild 했으므로 걍 가져오면댐

		const float r1 = Const::Tank::BodyWidth / 2.0f;					// Collider Radious
		const float r2 = bullet->getDiameter() / 2.0f;					// Bullet Radious

		if (p1.distance(p2) < (r1 + r2)) {
			return true;
		}
	}

	return false;
}

bool Tank::isDeath() {
	if (m_pInfo == nullptr) {
		return true;
	}

	return m_pInfo->IsDeath;
}
