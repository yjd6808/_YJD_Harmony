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

//////////////////////////////////////////////////////////////////////////////////////////
GunnerWalk::GunnerWalk(HostPlayer* _pHostPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pHostPlayer, _pActionInfo)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerWalk::onActionBegin()
{
	m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_WALK);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerWalk::onKeyPressed(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode)
{
	ActionMgr* pActionManager = m_pPlayer->actionManager();

	_pController->reflectPressedMoveKeys();

	if (_pController->getSpriteDirection() == SpriteDirection::Right &&
		_pController->isKeyPressed(ControlKey::Left))
		pActionManager->runBaseAction(BaseAction::Idle);
	else if (_pController->getSpriteDirection() == SpriteDirection::Left &&
		_pController->isKeyPressed(ControlKey::Right))
		pActionManager->runBaseAction(BaseAction::Idle);
	else if (isVerticalKeyPressed(_pController))
		pActionManager->runBaseAction(BaseAction::Idle);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerWalk::onKeyReleased(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode)
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
bool GunnerWalk::isHorizontalKeyPressed(PlayerController* _pController)
{
	if (_pController->isKeyPressed(ControlKey::Left) &&
		_pController->isKeyPressed(ControlKey::Right))
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GunnerWalk::isVerticalKeyPressed(PlayerController* _pController)
{
	if (_pController->isKeyPressed(ControlKey::Up) &&
		_pController->isKeyPressed(ControlKey::Down))
		return true;

	return false;
}
