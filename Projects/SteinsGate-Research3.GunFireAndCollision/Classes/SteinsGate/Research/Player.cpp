#include "Player.h"
/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 4:10:36 AM
 * =====================
 *
 */

using namespace cocos2d;

Player* Player::create(int type, int id)
{
	Player* player = new (std::nothrow) Player(type, id);
	player->init();
	player->autorelease();
	return player;
}

Player::Player(int type, int id) : Collider(type, id) {
	
}

Player::~Player() {
	CC_SAFE_RELEASE(m_pActionManager);
	CC_SAFE_RELEASE(m_pController);
	Log("플레이어 소멸\n");
}

bool Player::init() {

	if (!Collider::init())
		return false;

	m_pActionManager = PlayerActionManager::create(this);
	m_pActionManager->retain();

	m_pCharacterSprite = CharacterSprite::create(CharacterType::Gunner);
	m_pCharacterSprite->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	this->addChild(m_pCharacterSprite);

	m_pController = PlayerController::create(this, m_pCharacterSprite, m_pActionManager);
	m_pController->retain();

	
	

	return false;
}


float d = 0.0f;
void Player::update(float dt) {
	d += dt;

	m_pController->update(dt);
	if (d >= 0.5f) {
		d = 0.0f;

	}
}

void Player::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	m_pController->onKeyPressed(keyCode, event);
}

void Player::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	m_pController->onKeyReleased(keyCode, event);
}

SpriteDirection_t Player::getSpriteDirection() {
	return m_pController->getSpriteDirection();
}

cocos2d::Rect Player::getThicknessBox() {
	Vec2 thicknessAbsoluteOrigin = getRealPos();
	Rect thicknessRect{ thicknessAbsoluteOrigin, Size{ThicknessBoxWidth_v, ThicknessBoxHeight_v} };
	return thicknessRect;
}

cocos2d::Rect Player::getHitBox() {
	// 위치: 캔버스 좌하단 절대 좌표 + 캔버스 좌하단 기준 스킨 파츠 좌표
	//      캔버스 좌하단 절대 좌표 = 플레이어 Cocos 위치  + 캐릭터 위치 - (캔버스 사이즈 / 2)
	Vec2 absoluteOrigin = (this->getPosition() - m_pCharacterSprite->getCanvasSize() / 2);  // 캔버스 좌하단 절대 좌표
	Vec2 skinPartPos = m_pCharacterSprite->getPart(CharacterPartType::Skin)->getPosition(); // 캔버스 좌하단 기준 스킨 파츠 좌표
	DrawNode* pSkinBox = m_pCharacterSprite->getSkinBoundingBox();
	Size skinSize = pSkinBox->getContentSize();
	Rect hitBox{ absoluteOrigin + m_pCharacterSprite->getPosition() + skinPartPos, skinSize };
	return hitBox;
}

void Player::setRealPos(float x, float y) {
	x += ThicknessBoxWidth_v / 2.0f;
	y -= ThicknessRelativeY_v;
	y += ThicknessBoxHeight_v / 2.0f;
	setPosition(x, y);
}

cocos2d::Vec2 Player::getRealPos() {	// 두께박스 좌하단 좌표
	Vec2 thicknessOrigin = getPosition();
	thicknessOrigin.x -= ThicknessBoxWidth_v / 2.0f;
	thicknessOrigin.y += (ThicknessRelativeY_v - (ThicknessBoxHeight_v / 2.0f));
	return thicknessOrigin;
}

cocos2d::Vec2 Player::getTargetCanvasPosition() {
	return (this->getPosition() + m_pCharacterSprite->getPosition() - m_pCharacterSprite->getCanvasSize() / 2);
}


