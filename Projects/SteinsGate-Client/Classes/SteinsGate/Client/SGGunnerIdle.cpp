﻿/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGGunnerIdle.h"

#include <SteinsGate/Client/SGHostPlayer.h>
#include <SteinsGate/Client/SGAnimationDefine.h>

SGGunnerIdle::SGGunnerIdle(SGHostPlayer* player, SGActionInfo* actionInfo)
	: SGGunnerAction(player, actionInfo)
	, m_iIdleCount(0) {}


void SGGunnerIdle::init() {
	m_iIdleCount = 0;
}

void SGGunnerIdle::onActionBegin() {
	m_pPlayer->runAnimation(GUNNER_ANIMATION_IDLE_BREATH);
}

void SGGunnerIdle::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {
	int iAnimationCode = animation->getAnimationInfo()->Code;
	
	if (iAnimationCode == GUNNER_ANIMATION_IDLE_BREATH) {
		++m_iIdleCount;
	} else if (iAnimationCode == GUNNER_ANIMATION_IDLE_GUN_ROLLING) {
		m_pPlayer->runAnimation(GUNNER_ANIMATION_IDLE_BREATH);
		m_iIdleCount = 0;
	}
}

void SGGunnerIdle::onKeyPressed(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	controller->reflectPressedMoveKeys();
}

void SGGunnerIdle::onKeyReleasedBefore(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	ControlKey_t releasedKey = controller->convertControlKey(keyCode);

	if (releasedKey != ControlKey::None)
		fixFreezedState(controller, releasedKey);
}


void SGGunnerIdle::onActionEnd() {

}

void SGGunnerIdle::fixFreezedState(SGPlayerController* controller, ControlKey_t releasedKey) {
	ControlKey_t eReverseKey = ControlKey::ReverseDirection[releasedKey];
	SGActionManager* pActionManager = m_pPlayer->actionManager();

	if (controller->isKeyPressed(ControlKey::Left) && controller->isKeyPressed(ControlKey::Right) &&
		(releasedKey == ControlKey::Left || releasedKey == ControlKey::Right)) {

		controller->updateDirection(eReverseKey);
		pActionManager->runBaseAction(BaseAction::Walk);
	}

	if (controller->isKeyPressed(ControlKey::Up) && controller->isKeyPressed(ControlKey::Down) &&
		(releasedKey == ControlKey::Up || releasedKey == ControlKey::Down)) {

		controller->updateDirection(eReverseKey);
		pActionManager->runBaseAction(BaseAction::Walk);
	}
}