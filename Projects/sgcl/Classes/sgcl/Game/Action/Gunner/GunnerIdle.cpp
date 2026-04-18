/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */

#include "Game/Action/Gunner/GunnerIdle.h"

#include "sgcl/Game/Contents/HostPlayer.h"
#include "sgcl/Define/Define_Animation.h"

//////////////////////////////////////////////////////////////////////////////////////////
GunnerIdle::GunnerIdle(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pPlayer, _pActionInfo)
, idleCount_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerIdle::Init()
{
	idleCount_ = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerIdle::OnActionBegin()
{
	pPlayer_->RunAnimation(DEF_ANIMATION_GUNNER_IDLE_BREATH);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerIdle::OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
	int iAnimationCode = _pAnimation->GetAnimationInfo()->code_;

	if (iAnimationCode == DEF_ANIMATION_GUNNER_IDLE_BREATH)
	{
		++idleCount_;
	}
	else if (iAnimationCode == DEF_ANIMATION_GUNNER_IDLE_GUN_ROLLING)
	{
		pPlayer_->RunAnimation(DEF_ANIMATION_GUNNER_IDLE_BREATH);
		idleCount_ = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerIdle::OnKeyPressed(PlayerController* _pController, cc::EventKeyboard::KeyCode _keyCode)
{
	_pController->ReflectPressedMoveKeys();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerIdle::OnKeyReleasedBefore(PlayerController* _pController, cc::EventKeyboard::KeyCode _keyCode)
{
	ControlKey_t releasedKey = _pController->ConvertControlKey(_keyCode);

	if (releasedKey != ControlKey::None)
		FixFreezedState(_pController, releasedKey);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerIdle::OnActionEnd()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerIdle::FixFreezedState(PlayerController* _pController, ControlKey_t _releasedKey)
{
	ControlKey_t eReverseKey = ControlKey::ReverseDirection[_releasedKey];
	ActionMgr* pActionManager = pPlayer_->GetActionManager();

	if (_pController->IsKeyPressed(ControlKey::Left) && _pController->IsKeyPressed(ControlKey::Right) &&
		(_releasedKey == ControlKey::Left || _releasedKey == ControlKey::Right))
	{
		_pController->UpdateDirection(eReverseKey);
		pActionManager->RunBaseAction(BaseAction::Walk);
	}

	if (_pController->IsKeyPressed(ControlKey::Up) && _pController->IsKeyPressed(ControlKey::Down) &&
		(_releasedKey == ControlKey::Up || _releasedKey == ControlKey::Down))
	{
		_pController->UpdateDirection(eReverseKey);
		pActionManager->RunBaseAction(BaseAction::Walk);
	}
}
