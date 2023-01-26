/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */


#include "SGGunnerRun.h"

#include <SteinsGate/Research/SGPlayer.h>
#include <SteinsGate/Research/SGAnimationDefine.h>
#include <SteinsGate/Research/SGPlayerController.h>

SGGunnerRun::SGGunnerRun(SGPlayer* player, SGActionInfo* actionInfo)
	: SGComboAction(player, actionInfo)
{
}

void SGGunnerRun::onActionBegin() {
	m_pPlayer->getCharacter()->runAnimation(GUNNER_ANIMATION_RUN);
}

void SGGunnerRun::onKeyPressed(SGPlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {
	SGActionManager* pActionManager = m_pPlayer->getActionManager();

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

void SGGunnerRun::onKeyReleased(SGPlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {

	ControlKey_t releasedKey = controller->getLastestReleasedKey();
	SGActionManager* pActionManager = m_pPlayer->getActionManager();

	if (controller->isMoveKeyPressed() == false) {
		pActionManager->runBaseAction(BaseAction::Idle);
		controller->updateDirection(releasedKey);
	}

	
}

void SGGunnerRun::onKeyReleasedBefore(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) {

	ControlKey_t releasedKey = controller->convertControlKey(keyCode);

	if (releasedKey == ControlKey::None) return;
	if (controller->isKeyPressed(ControlKey::Left) && controller->isKeyPressed(ControlKey::Right) &&
		(releasedKey == ControlKey::Left || releasedKey == ControlKey::Right)) {
	}

}

