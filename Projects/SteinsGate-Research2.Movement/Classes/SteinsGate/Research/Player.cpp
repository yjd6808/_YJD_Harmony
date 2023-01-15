#include "Player.h"
/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 4:10:36 AM
 * =====================
 *
 */

Player* Player::create()
{
	Player* player = new Player;
	player->init();
	player->autorelease();
	return player;
}

bool Player::init() {
	m_pCharacterSprite = CharacterSprite::create(CharacterType::Gunner);
	m_pController = PlayerController::create(m_pCharacterSprite);

	this->addChild(m_pCharacterSprite);
	this->addChild(m_pController);

	return false;
}

void Player::update(float dt) {
	m_pController->update(dt);
}

void Player::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	m_pController->onKeyPressed(keyCode, event);
}

void Player::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	m_pController->onKeyReleased(keyCode, event);
}


void Player::onFrameBegin(int frameIndex, int frameIndexInAnimation) {
}
