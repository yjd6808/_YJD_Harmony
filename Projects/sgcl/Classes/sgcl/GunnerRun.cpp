/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */

#include "Core.h"
#include "GunnerRun.h"

#include <sgcl/HostPlayer.h>
#include <sgcl/Define_Animation.h>
#include <sgcl/PlayerController.h>

//////////////////////////////////////////////////////////////////////////////////////////
GunnerRun::GunnerRun(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pPlayer, _pActionInfo)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerRun::OnActionBegin()
{
	pPlayer_->RunAnimation(DEF_ANIMATION_GUNNER_RUN);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerRun::OnKeyPressed(PlayerController* _pController, cocos2d::EventKeyboard::KeyCode _keyCode)
{
	ActionMgr* pActionManager = pPlayer_->GetActionManager();

	if (_pController->GetSpriteDirection() == SpriteDirection::Right &&
		_pController->IsKeyPressed(ControlKey::Left))
	{
		_pController->UpdateDirection(ControlKey::Left);
		pActionManager->RunBaseAction(BaseAction::Walk);
	}
	else if (_pController->GetSpriteDirection() == SpriteDirection::Left &&
		_pController->IsKeyPressed(ControlKey::Right))
	{
		_pController->UpdateDirection(ControlKey::Right);
		pActionManager->RunBaseAction(BaseAction::Walk);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerRun::OnKeyReleased(PlayerController* _pController, cocos2d::EventKeyboard::KeyCode _keyCode)
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
void GunnerRun::OnKeyReleasedBefore(PlayerController* _pController, cc::EventKeyboard::KeyCode _keyCode)
{
	ControlKey_t releasedKey = _pController->ConvertControlKey(_keyCode);

	if (releasedKey == ControlKey::None)
		return;

	if (_pController->IsKeyPressed(ControlKey::Left) && _pController->IsKeyPressed(ControlKey::Right) &&
		(releasedKey == ControlKey::Left || releasedKey == ControlKey::Right))
	{
	}
}
