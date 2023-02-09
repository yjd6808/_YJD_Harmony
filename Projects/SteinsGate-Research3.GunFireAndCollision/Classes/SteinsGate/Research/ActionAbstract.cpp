/*
 * 작성자: 윤정도
 * 생성일: 1/13/2023 11:42:40 PM
 * =====================
 *
 */

#include "ActionAbstract.h"

#include <SteinsGate/Research/ComboTree.h>
#include <JCore/Utils/Console.h>

#include "Player.h"

void ActionAbstract::play() {
	//m_fMoveSpeedFPSY = DefaultMoveSpeedPerFPSY_v;
	//m_fMoveSpeedFPSX = DefaultMoveSpeedPerFPSX_v;

	onActionBegin();
}

void ActionAbstract::stop() {
	m_pPlayer->getPlayerActionManager()->setRunningAction(nullptr);	// 완료된 액션은 먼저 바로 해제 해준다.
	m_pPlayer->getController()->setCommandable(true);				// 다시 콤보 입력이 가능하도록 변경해준다.
	m_pPlayer->getController()->reflectPressedMoveKeys();

	onActionEnd();
}

void ActionAbstract::onKeyPressed(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {
	
}

void ActionAbstract::onKeyReleased(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {
	
}


void ActionAbstract::onKeyPressedBefore(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {
	
}

void ActionAbstract::onKeyReleasedBefore(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {
	
}


void ActionAbstract::onFrameBegin(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame) {
	
}

void ActionAbstract::onFrameEnd(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame) {
	
}

void ActionAbstract::onAnimateBegin(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame) {
	
}

void ActionAbstract::onAnimateEnd(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame) {
	
}

void ActionAbstract::onActionBegin() {
#ifdef DebugMode
	if (isComboAction()) {
		ComboAction* pComboAction = (ComboAction*)this;
		Log("%s", pComboAction->getComboKeys().String().Source());
	}
#endif
}


void ActionAbstract::onActionEnd() {
}
