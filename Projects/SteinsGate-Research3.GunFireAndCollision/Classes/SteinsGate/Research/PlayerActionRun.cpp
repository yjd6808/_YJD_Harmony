/*
 * 작성자: 윤정도
 * 생성일: 1/14/2023 7:07:26 AM
 * =====================
 *
 */


#include "PlayerActionRun.h"

#include <SteinsGate/Research/Player.h>

using namespace cocos2d;

PlayerActionRun* PlayerActionRun::createRetain(SpriteDirection_t direction) {
	auto action = new PlayerActionRun(direction);

	if (action->init()) {
		action->autorelease();
		action->retain();
		return action;
	}

	DebugAssertMessage(false, "초기화에 실패하였습니다.");
	CC_SAFE_DELETE(action);
	return nullptr;
}

bool PlayerActionRun::init() {
	switch (m_eDirection) {
		case SpriteDirection::Left:  {
			initWithKeys({ ControlKey::Left, ControlKey::Left });
			m_iActionType = ActionType::RunLeft;
			break;
		}
		case SpriteDirection::Right: {
			initWithKeys({ ControlKey::Right, ControlKey::Right });
			m_iActionType = ActionType::RunRight;
			break;
		}
		default: return false;
	}

	m_fMoveSpeed = DefaultRunSpeedPerFPSX_v;
	m_bMoveable = true;
	m_bCancelable = true;
	return true;
}

void PlayerActionRun::onActionBegin() {
	m_pPlayer->getCharacterSprite()->setRunning();
}

void PlayerActionRun::onKeyPressed(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {
	if (controller->isHorizontalKeyPressed()) {
		stop();
	}
}

void PlayerActionRun::onKeyReleased(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {

	ControlKey_t releasedKey = controller->getLastestReleasedKey();

	if (controller->isMoveKeyPressed() == false) {
		stop();
		controller->updateDirection(releasedKey);
	}
}

void PlayerActionRun::onKeyReleasedBefore(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {

	ControlKey_t releasedKey = controller->convertControlKey(keyCode);

	if (controller->isFreezedWalkState(releasedKey)) {
		stop();	
	}
}
