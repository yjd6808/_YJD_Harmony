/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 10:25:01 AM
 * =====================
 *
 */


#include "SGPlayer.h"

SGPlayer::SGPlayer()
	: m_pCharacter(nullptr) {
	
}

SGPlayer::~SGPlayer() {
	CC_SAFE_RELEASE(m_pCharacter);
}

void SGPlayer::update(float dt) {
	m_pCharacter->update(dt);
}

void SGPlayer::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {

}

void SGPlayer::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	
}

void SGPlayer::setCharacter(SGCharacter* character) { m_pCharacter = character; }

SGCharacter* SGPlayer::getCharacter() { return m_pCharacter; }

