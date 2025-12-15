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

//////////////////////////////////////////////////////////////////////////////////////////
GunnerIdle::GunnerIdle(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pPlayer, _pActionInfo)
, idleCount_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerIdle::init()
{
	idleCount_ = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerIdle::onActionBegin()
{
	m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_IDLE_BREATH);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerIdle::onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
	int iAnimationCode = _pAnimation->getAnimationInfo()->code_;

	if (iAnimationCode == DEF_ANIMATION_GUNNER_IDLE_BREATH)
	{
		++idleCount_;
	}
	else if (iAnimationCode == DEF_ANIMATION_GUNNER_IDLE_GUN_ROLLING)
	{
		m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_IDLE_BREATH);
		idleCount_ = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerIdle::onKeyPressed(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode)
{
	_pController->reflectPressedMoveKeys();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerIdle::onKeyReleasedBefore(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode)
{
	ControlKey_t releasedKey = _pController->convertControlKey(_keyCode);

	if (releasedKey != ControlKey::None)
		FixFreezedState(_pController, releasedKey);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerIdle::onActionEnd()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerIdle::FixFreezedState(PlayerController* _pController, ControlKey_t _releasedKey)
{
	ControlKey_t eReverseKey = ControlKey::ReverseDirection[_releasedKey];
	ActionMgr* pActionManager = m_pPlayer->actionManager();

	if (_pController->isKeyPressed(ControlKey::Left) && _pController->isKeyPressed(ControlKey::Right) &&
		(_releasedKey == ControlKey::Left || _releasedKey == ControlKey::Right))
	{
		_pController->updateDirection(eReverseKey);
		pActionManager->runBaseAction(BaseAction::Walk);
	}

	if (_pController->isKeyPressed(ControlKey::Up) && _pController->isKeyPressed(ControlKey::Down) &&
		(_releasedKey == ControlKey::Up || _releasedKey == ControlKey::Down))
	{
		_pController->updateDirection(eReverseKey);
		pActionManager->runBaseAction(BaseAction::Walk);
	}
}
