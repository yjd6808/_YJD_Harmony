#include <TF/Object/Bullet.h>
#include <TF/Util/ColorUtil.h>

Bullet* Bullet::create(float diameter, int characterUID) {
	Bullet* sprite = new (std::nothrow) Bullet(characterUID);

	if (sprite && sprite->init(diameter) && sprite->initWithFile(CIRCLE_BIG_IMG_FILENAME)) {
		sprite->autorelease();
		return sprite;
	}

	CC_SAFE_DELETE(sprite);
	return nullptr;
}

bool Bullet::init(float diameter) {
	if (!Sprite::init()) {
		return false;
	}

	auto imgSize = this->getBoundingBox().size;
	this->scheduleUpdate();

	this->m_BulletInfo.Diameter = diameter;
	this->setScale(diameter / CIRCLE_BIG_IMG_SIZE);

	return true;
}

void Bullet::update(float delta) {
	Vec2 curPos = this->getPosition();
	curPos.x += m_BulletInfo.MoveSpeed * sinf(CC_DEGREES_TO_RADIANS(this->getRotation())) * delta;
	curPos.y += m_BulletInfo.MoveSpeed * cosf(CC_DEGREES_TO_RADIANS(this->getRotation())) * delta;
	this->setPosition(curPos);
	m_BulletInfo.X = curPos.x;
	m_BulletInfo.Y = curPos.y;
}

void Bullet::UpdateBulletInfo(BulletInfo& info) {
	Memory::CopyUnsafe(&m_BulletInfo, &info, sizeof(BulletInfo));

	this->setPosition({ info.X, info.Y });
	this->setRotation(info.Rotation);
	Color3B z = ColorUtil::To3B(info.Color);
	this->setColor(z);
	this->setScale(info.Diameter / CIRCLE_BIG_IMG_SIZE);
}

void Bullet::LoadBulletInfo(BulletInfo& info) {
	info.X = this->getPosition().x;
	info.Y = this->getPosition().y;
	info.Rotation = this->getRotation();
	info.CharacterUID = this->GetCharacterUID();
	info.Color = ColorUtil::ToIn32U(this->getColor());
	info.MoveSpeed = m_BulletInfo.MoveSpeed;
	info.Diameter = m_BulletInfo.Diameter;
}
