#include "Player.h"
/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 4:10:36 AM
 * =====================
 *
 */

using namespace cocos2d;

Player* Player::create()
{
	Player* player = new Player;
	player->init();
	player->autorelease();
	return player;
}

Player::~Player() {
	CC_SAFE_RELEASE(m_pActionManager);
	CC_SAFE_RELEASE(m_pController);
}

bool Player::init() {
	m_pActionManager = PlayerActionManager::create(this);
	m_pActionManager->retain();

	m_pCharacterSprite = CharacterSprite::create(CharacterType::Gunner);
	m_pCharacterSprite->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	this->addChild(m_pCharacterSprite);

	m_pController = PlayerController::create(this, m_pCharacterSprite, m_pActionManager);
	m_pController->retain();

	Vec2 polygon[4];
	polygon[0] = Vec2(-ThicknessBoxWidth_v / 2.0f,  ThicknessBoxHeight_v / 2.0f);
	polygon[1] = Vec2( ThicknessBoxWidth_v / 2.0f , ThicknessBoxHeight_v / 2.0f);
	polygon[2] = Vec2( ThicknessBoxWidth_v / 2.0f ,-ThicknessBoxHeight_v / 2.0f);
	polygon[3] = Vec2(-ThicknessBoxWidth_v / 2.0f, -ThicknessBoxHeight_v / 2.0f);

	// 앵커포인터 0, 0으로 초기화됨; 스프라이트만 0.5인듯
	// 제발 살려줘 코코스야..
	m_pThicknessBox = cocos2d::DrawNode::create();	
	m_pThicknessBox->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_pThicknessBox->setPositionY(ThicknessRelativeY_v);
	m_pThicknessBox->drawPolygon(polygon, 4, {}, 1, Color4F::MAGENTA);
	this->addChild(m_pThicknessBox);

	

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


