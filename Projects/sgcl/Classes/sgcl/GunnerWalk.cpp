/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */

#include "Core.h"
#include "GunnerWalk.h"

#include <sgcl/HostPlayer.h>
#include <sgcl/Define_Animation.h>

//////////////////////////////////////////////////////////////////////////////////////////
GunnerWalk::GunnerWalk(HostPlayer* _pHostPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pHostPlayer, _pActionInfo)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerWalk::OnActionBegin()
{
	pPlayer_->RunAnimation(DEF_ANIMATION_GUNNER_WALK);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerWalk::OnKeyPressed(PlayerController* _pController, cc::EventKeyboard::KeyCode _keyCode)
{
	ActionMgr* pActionManager = pPlayer_->GetActionManager();

	_pController->ReflectPressedMoveKeys();

	if (_pController->GetSpriteDirection() == SpriteDirection::Right &&
		_pController->IsKeyPressed(ControlKey::Left))
		pActionManager->RunBaseAction(BaseAction::Idle);
	else if (_pController->GetSpriteDirection() == SpriteDirection::Left &&
		_pController->IsKeyPressed(ControlKey::Right))
		pActionManager->RunBaseAction(BaseAction::Idle);
	else if (IsVerticalKeyPressed(_pController))
		pActionManager->RunBaseAction(BaseAction::Idle);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerWalk::OnKeyReleased(PlayerController* _pController, cc::EventKeyboard::KeyCode _keyCode)
{
	ControlKey_t releasedKey = _pController->GetLastestReleasedKey();
	ActionMgr* pActionManager = pPlayer_->GetActionManager();

	if (_pController->IsMoveKeyPressed() == false)
	{
		pActionManager->RunBaseAction(BaseAction::Idle);
		_pController->UpdateDirection(releasedKey);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GunnerWalk::IsHorizontalKeyPressed(PlayerController* _pController)
{
	if (_pController->IsKeyPressed(ControlKey::Left) &&
		_pController->IsKeyPressed(ControlKey::Right))
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GunnerWalk::IsVerticalKeyPressed(PlayerController* _pController)
{
	if (_pController->IsKeyPressed(ControlKey::Up) &&
		_pController->IsKeyPressed(ControlKey::Down))
		return true;

	return false;
}
