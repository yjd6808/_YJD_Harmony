/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 9:07:50 AM
 * =====================
 *
 */

#include "Core.h"
#include "SGAction.h"
#include "GameCoreHeader.h"

#include <sgcl/HostPlayer.h>
#include <jc/Utils/Console.h>

//////////////////////////////////////////////////////////////////////////////////////////
SGAction::SGAction(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: pPlayer_(_pPlayer)
, pActionInfo_(_pActionInfo)
, pHitRecorder_(nullptr)
, isMoveablePositiveX_(pActionInfo_->Moveable)
, isMoveablePositiveY_(pActionInfo_->Moveable)
, isMoveableNegativeX_(pActionInfo_->Moveable)
, isMoveableNegativeY_(pActionInfo_->Moveable)
, isCancelable_(pActionInfo_->ForceCancelable)
, moveSpeedFPSX(pActionInfo_->SpeedX)
, moveSpeedFPSY_(pActionInfo_->SpeedY)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SGAction::~SGAction()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::Play()
{
	// 플레이어가 사용가능한지 체크
	pHitRecorder_ = pPlayer_->GetHitRecorder();
	pHitRecorder_->ClearAlreadyHitEnemies();
	weaponType_ = sg::Contents.Inven->GetWeaponType();

	OnActionBegin();
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::Stop()
{
	pPlayer_->GetActionManager()->StopActionForce();
	pPlayer_->GetController()->SetCommandable(true); // 다시 콤보 입력이 가능하도록 변경해준다.

	pHitRecorder_->SetAlreadyHitRecord(false);
	pHitRecorder_->ClearAlreadyHitEnemies();
	pHitRecorder_->ClearSingleHitCallback();
	pHitRecorder_->ClearMultiHitCallback();

	if (!IsForceCancelable())
	{
		pPlayer_->GetController()->ReflectPressedMoveKeys();
	}

	OnActionEnd();
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::OnActionBegin()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::OnActionEnd()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::OnKeyPressed(PlayerController* _pController, cocos2d::EventKeyboard::KeyCode _keyCode)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::OnKeyReleased(PlayerController* _pController, cocos2d::EventKeyboard::KeyCode _keyCode)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::OnKeyPressedBefore(PlayerController* _pController, cocos2d::EventKeyboard::KeyCode _keyCode)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::OnKeyReleasedBefore(PlayerController* _pController, cocos2d::EventKeyboard::KeyCode _keyCode)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGAction::SetMoveable(bool _moveable)
{
	isMoveablePositiveX_ = _moveable;
	isMoveablePositiveY_ = _moveable;
	isMoveableNegativeX_ = _moveable;
	isMoveableNegativeY_ = _moveable;
}
