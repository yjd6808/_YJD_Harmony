/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGGunnerWalk.h"

#include <SteinsGate/Research/SGHostPlayer.h>
#include <SteinsGate/Research/SGAnimationDefine.h>

SGGunnerWalk::SGGunnerWalk(SGHostPlayer* player, SGActionInfo* actionInfo)
	: SGGunnerAction(player, actionInfo) {}

void SGGunnerWalk::onActionBegin() {
	m_pPlayer->getCharacter()->runAnimation(GUNNER_ANIMATION_WALK);
}

void SGGunnerWalk::onKeyPressed(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	SGActionManager* pActionManager = m_pPlayer->getActionManager();

	controller->reflectPressedMoveKeys();

	if (controller->getSpriteDirection() == SpriteDirection::Right &&
		controller->isKeyPressed(ControlKey::Left))
		pActionManager->runBaseAction(BaseAction::Idle);
	else if (controller->getSpriteDirection() == SpriteDirection::Left &&
		controller->isKeyPressed(ControlKey::Right))
		pActionManager->runBaseAction(BaseAction::Idle);
	else if (isVerticalKeyPressed(controller))
		pActionManager->runBaseAction(BaseAction::Idle);
}

void SGGunnerWalk::onKeyReleased(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	ControlKey_t releasedKey = controller->getLastestReleasedKey();
	SGActionManager* pActionManager = m_pPlayer->getActionManager();

	if (controller->isMoveKeyPressed() == false) {
		pActionManager->runBaseAction(BaseAction::Idle);
		controller->updateDirection(releasedKey);
	}
}


bool SGGunnerWalk::isHorizontalKeyPressed(SGPlayerController* controller) {
	if (controller->isKeyPressed(ControlKey::Left) &&
		controller->isKeyPressed(ControlKey::Right))
		return true;
	return false;
}

bool SGGunnerWalk::isVerticalKeyPressed(SGPlayerController* controller) {
	if (controller->isKeyPressed(ControlKey::Up) &&
		controller->isKeyPressed(ControlKey::Down))
		return true;

	return false;
}

