/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GunnerIdle.h"

#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/Define_Animation.h>

GunnerIdle::GunnerIdle(HostPlayer* player, ActionInfo* actionInfo)
	: GunnerAction(player, actionInfo)
	, m_iIdleCount(0) {}


void GunnerIdle::init() {
	m_iIdleCount = 0;
}

void GunnerIdle::onActionBegin() {
	m_pPlayer->runAnimation(GUNNER_ANIMATION_IDLE_BREATH);
}

void GunnerIdle::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* frame) {
	int iAnimationCode = animation->getAnimationInfo()->Code;
	
	if (iAnimationCode == GUNNER_ANIMATION_IDLE_BREATH) {
		++m_iIdleCount;
	} else if (iAnimationCode == GUNNER_ANIMATION_IDLE_GUN_ROLLING) {
		m_pPlayer->runAnimation(GUNNER_ANIMATION_IDLE_BREATH);
		m_iIdleCount = 0;
	}
}

void GunnerIdle::onKeyPressed(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	controller->reflectPressedMoveKeys();
}

void GunnerIdle::onKeyReleasedBefore(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	ControlKey_t releasedKey = controller->convertControlKey(keyCode);

	if (releasedKey != ControlKey::None)
		fixFreezedState(controller, releasedKey);
}


void GunnerIdle::onActionEnd() {

}

void GunnerIdle::fixFreezedState(PlayerController* controller, ControlKey_t releasedKey) {
	ControlKey_t eReverseKey = ControlKey::ReverseDirection[releasedKey];
	ActionMgr* pActionManager = m_pPlayer->actionManager();

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
