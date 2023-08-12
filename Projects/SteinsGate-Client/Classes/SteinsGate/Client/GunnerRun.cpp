/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GunnerRun.h"

#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/Define_Animation.h>
#include <SteinsGate/Client/PlayerController.h>

GunnerRun::GunnerRun(HostPlayer* player, ActionInfo* actionInfo)
	: GunnerAction(player, actionInfo)
{
}

void GunnerRun::onActionBegin() {
	m_pPlayer->runAnimation(GUNNER_ANIMATION_RUN);
}

void GunnerRun::onKeyPressed(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {
	ActionMgr* pActionManager = m_pPlayer->actionManager();

	if (controller->getSpriteDirection() == SpriteDirection::Right &&
		controller->isKeyPressed(ControlKey::Left)) {
		controller->updateDirection(ControlKey::Left);
		pActionManager->runBaseAction(BaseAction::Walk);
	}
	else if (controller->getSpriteDirection() == SpriteDirection::Left &&
		controller->isKeyPressed(ControlKey::Right)) {
		controller->updateDirection(ControlKey::Right);
		pActionManager->runBaseAction(BaseAction::Walk);
	}

}

void GunnerRun::onKeyReleased(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {

	ControlKey_t releasedKey = controller->getLastestReleasedKey();
	ActionMgr* pActionManager = m_pPlayer->actionManager();

	if (controller->isMoveKeyPressed() == false) {
		pActionManager->runBaseAction(BaseAction::Idle);
		controller->updateDirection(releasedKey);
	}

	
}

void GunnerRun::onKeyReleasedBefore(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) {

	ControlKey_t releasedKey = controller->convertControlKey(keyCode);

	if (releasedKey == ControlKey::None) return;
	if (controller->isKeyPressed(ControlKey::Left) && controller->isKeyPressed(ControlKey::Right) &&
		(releasedKey == ControlKey::Left || releasedKey == ControlKey::Right)) {
	}

}

