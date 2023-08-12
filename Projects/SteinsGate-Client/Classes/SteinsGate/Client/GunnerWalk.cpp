/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GunnerWalk.h"

#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/Define_Animation.h>

GunnerWalk::GunnerWalk(HostPlayer* player, ActionInfo* actionInfo)
	: GunnerAction(player, actionInfo) {}

void GunnerWalk::onActionBegin() {
	m_pPlayer->runAnimation(GUNNER_ANIMATION_WALK);
}

void GunnerWalk::onKeyPressed(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	ActionMgr* pActionManager = m_pPlayer->actionManager();

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

void GunnerWalk::onKeyReleased(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	ControlKey_t releasedKey = controller->getLastestReleasedKey();
	ActionMgr* pActionManager = m_pPlayer->actionManager();

	if (controller->isMoveKeyPressed() == false) {
		pActionManager->runBaseAction(BaseAction::Idle);
		controller->updateDirection(releasedKey);
	}
}


bool GunnerWalk::isHorizontalKeyPressed(PlayerController* controller) {
	if (controller->isKeyPressed(ControlKey::Left) &&
		controller->isKeyPressed(ControlKey::Right))
		return true;
	return false;
}

bool GunnerWalk::isVerticalKeyPressed(PlayerController* controller) {
	if (controller->isKeyPressed(ControlKey::Up) &&
		controller->isKeyPressed(ControlKey::Down))
		return true;

	return false;
}

