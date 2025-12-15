/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 9:07:50 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGAction.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/HostPlayer.h>
#include <JCore/Utils/Console.h>

//////////////////////////////////////////////////////////////////////////////////////////
SGAction::SGAction(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: m_pPlayer(_pPlayer)
, m_pActionInfo(_pActionInfo)
, m_pHitRecorder(nullptr)
, m_bMoveablePositiveX(m_pActionInfo->Moveable)
, m_bMoveablePositiveY(m_pActionInfo->Moveable)
, m_bMoveableNegativeX(m_pActionInfo->Moveable)
, m_bMoveableNegativeY(m_pActionInfo->Moveable)
, m_bCancelable(m_pActionInfo->ForceCancelable)
, m_fMoveSpeedFPSX(m_pActionInfo->SpeedX)
, m_fMoveSpeedFPSY(m_pActionInfo->SpeedY)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SGAction::~SGAction()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::play()
{
	// 플레이어가 사용가능한지 체크
	m_pHitRecorder = m_pPlayer->getHitRecorder();
	m_pHitRecorder->clearAlreadyHitEnemies();
	m_eWeaponType = Core::Contents.Inven->getWeaponType();

	onActionBegin();
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::stop()
{
	m_pPlayer->actionManager()->stopActionForce();
	m_pPlayer->ctrl()->setCommandable(true); // 다시 콤보 입력이 가능하도록 변경해준다.

	m_pHitRecorder->setAlreadyHitRecord(false);
	m_pHitRecorder->clearAlreadyHitEnemies();
	m_pHitRecorder->clearSingleHitCallback();
	m_pHitRecorder->clearMultiHitCallback();

	if (!isForceCancelable())
	{
		m_pPlayer->ctrl()->reflectPressedMoveKeys();
	}

	onActionEnd();
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::onActionBegin()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::onActionEnd()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::onKeyPressed(PlayerController* _pController, cocos2d::EventKeyboard::KeyCode _keyCode)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::onKeyReleased(PlayerController* _pController, cocos2d::EventKeyboard::KeyCode _keyCode)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::onKeyPressedBefore(PlayerController* _pController, cocos2d::EventKeyboard::KeyCode _keyCode)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::onKeyReleasedBefore(PlayerController* _pController, cocos2d::EventKeyboard::KeyCode _keyCode)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::setMoveable(bool _moveable)
{
	m_bMoveablePositiveX = _moveable;
	m_bMoveablePositiveY = _moveable;
	m_bMoveableNegativeX = _moveable;
	m_bMoveableNegativeY = _moveable;
}
