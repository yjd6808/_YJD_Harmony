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

//////////////////////////////////////////////////////////////////////////////////////////
GunnerRun::GunnerRun(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pPlayer, _pActionInfo)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerRun::onActionBegin()
{
	m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_RUN);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerRun::onKeyPressed(PlayerController* _pController, cocos2d::EventKeyboard::KeyCode _keyCode)
{
	ActionMgr* pActionManager = m_pPlayer->actionManager();

	if (_pController->getSpriteDirection() == SpriteDirection::Right &&
		_pController->isKeyPressed(ControlKey::Left))
	{
		_pController->updateDirection(ControlKey::Left);
		pActionManager->runBaseAction(BaseAction::Walk);
	}
	else if (_pController->getSpriteDirection() == SpriteDirection::Left &&
		_pController->isKeyPressed(ControlKey::Right))
	{
		_pController->updateDirection(ControlKey::Right);
		pActionManager->runBaseAction(BaseAction::Walk);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerRun::onKeyReleased(PlayerController* _pController, cocos2d::EventKeyboard::KeyCode _keyCode)
{
	ControlKey_t releasedKey = _pController->getLastestReleasedKey();
	ActionMgr* pActionManager = m_pPlayer->actionManager();

	if (_pController->isMoveKeyPressed() == false)
	{
		pActionManager->runBaseAction(BaseAction::Idle);
		_pController->updateDirection(releasedKey);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerRun::onKeyReleasedBefore(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode)
{
	ControlKey_t releasedKey = _pController->convertControlKey(_keyCode);

	if (releasedKey == ControlKey::None)
		return;

	if (_pController->isKeyPressed(ControlKey::Left) && _pController->isKeyPressed(ControlKey::Right) &&
		(releasedKey == ControlKey::Left || releasedKey == ControlKey::Right))
	{
	}
}
