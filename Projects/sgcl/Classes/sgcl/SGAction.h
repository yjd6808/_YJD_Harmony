/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 9:06:49 AM
 * =====================
 *
 */


#pragma once

#include "sg/_Struct/SteinsGate_Action.h"
#include "sgcl/HitRecorder.h"

struct ActionInfo;

class HostPlayer;
class PlayerController;
class FrameTexture;
class Actor;
class ActorPartAnimation;

class SGAction
{
public:
	SGAction(HostPlayer* _pPlayer, ActionInfo* _pActionInfo);
	virtual ~SGAction();

	virtual void Play();
	virtual void Stop();

	virtual void Init()
	{
	} // 실행 직전 호출 (초기화 용도)
	virtual bool OnConditionCheck() = 0;
	virtual void OnKeyPressed(PlayerController* _pController, cc::EventKeyboard::KeyCode _keyCode);
	virtual void OnKeyReleased(PlayerController* _pController, cc::EventKeyboard::KeyCode _keyCode);
	virtual void OnKeyPressedBefore(PlayerController* _pController, cc::EventKeyboard::KeyCode _keyCode);
	virtual void OnKeyReleasedBefore(PlayerController* _pController, cc::EventKeyboard::KeyCode _keyCode);
	virtual void OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame);
	virtual void OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame);
	virtual void OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame);
	virtual void OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame);
	virtual void OnActionBegin();
	virtual void OnActionEnd(); // 정의안하면 해당 애니메이션 시퀀스가 끝나면 중단토록 한다.
	virtual void OnUpdate(float _dt)
	{
	}

	// virtual void onHit();
	// virtual void onAttack();

	virtual bool IsComboAction() { return false; }
	virtual bool IsAttackAction() { return false; }

	int GetActionCode() { return pActionInfo_->code_; }
	const jc::String& GetActionName() { return pActionInfo_->ActionName; }
	const ComboKeyList& GetCommand() { return pActionInfo_->Command; }
	float GetMoveSpeedX() { return moveSpeedFPSX; }
	float GetMoveSpeedY() { return moveSpeedFPSY_; }

	bool IsForceCancelable() { return isCancelable_; }

	bool IsMovealbe()
	{
		return
			isMoveablePositiveX_ &&
			isMoveablePositiveY_ &&
			isMoveableNegativeX_ &&
			isMoveableNegativeY_;
	}

	bool IsMoveablePositiveX() { return isMoveablePositiveX_; }
	bool IsMoveablePositiveY() { return isMoveablePositiveY_; }
	bool IsMoveableNegativeX() { return isMoveableNegativeX_; }
	bool IsMoveableNegativeY() { return isMoveableNegativeY_; }

	void SetMoveable(bool _moveable);

	void InitHitRecorder(
		const SGHitSingleCallbackFn& _singleHitFn,
		const SGHitMultiCallbackFn& _multiHitFn,
		int _hitListSize = 16,
		int _alreadyHitListSize = 32);

protected:
	// 주입 하는 필드
	HostPlayer* pPlayer_;
	ActionInfo* pActionInfo_;

	// 자체 필드
	HitRecorder* pHitRecorder_;
	WeaponType_t weaponType_;

	bool isMoveablePositiveX_;
	bool isMoveablePositiveY_;

	bool isMoveableNegativeX_;
	bool isMoveableNegativeY_;

	bool isCancelable_;
	float moveSpeedFPSX;
	float moveSpeedFPSY_;
};
