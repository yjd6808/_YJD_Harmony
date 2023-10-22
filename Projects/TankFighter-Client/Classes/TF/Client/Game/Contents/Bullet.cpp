/*
 * 작성자: 윤정도
 * 생성일: 10/20/2023 5:52:31 PM
 * =====================
 *
 */

#include "Pch.h"
#include "Bullet.h"

USING_NS_JC;
USING_NS_JNET;

Bullet::Bullet()
	: m_Info()
{}

Bullet* Bullet::create() {
	Bullet* sprite = dbg_new Bullet();

	if (sprite && sprite->init() && sprite->initWithFile(Const::Resource::WhiteCircleFileName)) {
		sprite->autorelease();
		return sprite;
	}

	CC_SAFE_DELETE(sprite);
	return nullptr;
}

bool Bullet::init() {
	if (!Scale9Sprite::init()) {
		return false;
	}

	return true;
}

bool Bullet::isOutOfMap() const {
	return	_position.x < 0 || _position.x > Const::Map::Width || 
			_position.y < 0 || _position.y > Const::Map::Height;
}

void Bullet::update(float delta) {
	cocos2d::Vec2 curPos = getPosition();
	curPos.x += m_Info.MoveSpeed * sinf(CC_DEGREES_TO_RADIANS(getRotation())) * delta;
	curPos.y += m_Info.MoveSpeed * cosf(CC_DEGREES_TO_RADIANS(getRotation())) * delta;
	setPosition(curPos);
	m_Info.X = curPos.x;
	m_Info.Y = curPos.y;
}

void Bullet::setCharacterPrimaryKey(int primaryKey) {
	m_Info.CharacterPrimaryKey = primaryKey;
}

void Bullet::setDiameter(float diameter) {
	m_Info.Diameter = diameter;
	setContentSize({ diameter, diameter });
}

void Bullet::setNetInfo(const BulletInfoNet& bulletInfo) {
	m_Info = bulletInfo;
	setContentSize({ m_Info.Diameter, m_Info.Diameter });
	setPosition({ m_Info.X, m_Info.Y });
	setRotation(m_Info.Rotation);
	setColor(ColorUtil::To3B(m_Info.Color));
}

BulletInfoNet Bullet::getNetInfo() const {
	BulletInfoNet info;
	info.X = getPositionX();
	info.Y = getPositionY();
	info.Rotation = getRotation();
	info.CharacterPrimaryKey = m_Info.CharacterPrimaryKey;
	info.Color = ColorUtil::ToIn32U(getColor());
	info.MoveSpeed = m_Info.MoveSpeed;
	info.Diameter = m_Info.Diameter;
	return info;
}

