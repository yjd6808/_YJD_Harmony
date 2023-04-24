/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 9:06:49 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/ActionInfo.h>
#include <SteinsGate/Client/HitRecorder.h>

struct ActionInfo;

class HostPlayer;
class PlayerController;
class FrameTexture;
class Actor;
class ActorPartAnimation;
class SGAction {
public:
	SGAction(HostPlayer* player, ActionInfo* actionInfo);
	virtual ~SGAction();

	virtual void play();
	virtual void stop();

	virtual void init() {}	// 실행 직전 호출 (초기화 용도)
	virtual bool onConditionCheck() = 0;
	virtual void onKeyPressed(PlayerController* controller, SGEventKeyboard::KeyCode keyCode);
	virtual void onKeyReleased(PlayerController* controller, SGEventKeyboard::KeyCode keyCode);
	virtual void onKeyPressedBefore(PlayerController* controller, SGEventKeyboard::KeyCode keyCode);
	virtual void onKeyReleasedBefore(PlayerController* controller, SGEventKeyboard::KeyCode keyCode);
	virtual void onFrameBegin(ActorPartAnimation* animation, FrameTexture* frame);
	virtual void onFrameEnd(ActorPartAnimation* animation, FrameTexture* frame);
	virtual void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* frame);
	virtual void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* frame);
	virtual void onActionBegin();
	virtual void onActionEnd(); 	// 정의안하면 해당 애니메이션 시퀀스가 끝나면 중단토록 한다.
	virtual void onUpdate(float dt) {}

	// virtual void onHit();
	// virtual void onAttack();

	virtual bool isComboAction() { return false; }
	virtual bool isAttackAction() { return false; }
	
	int getActionCode()  { return m_pActionInfo->Code; }
	const SGString& getActionName() { return m_pActionInfo->ActionName; }
	const ComboKeyList& getCommand() { return m_pActionInfo->Command; }
	float getMoveSpeedX() { return m_fMoveSpeedFPSX; }
	float getMoveSpeedY() { return m_fMoveSpeedFPSY; }

	bool isForceCancelable() { return m_bCancelable; }
	bool isMovealbe() { return
		m_bMoveablePositiveX && 
		m_bMoveablePositiveY && 
		m_bMoveableNegativeX &&
		m_bMoveableNegativeY; }

	bool isMoveablePositiveX() { return m_bMoveablePositiveX; }
	bool isMoveablePositiveY() { return m_bMoveablePositiveY; }
	bool isMoveableNegativeX() { return m_bMoveableNegativeX; }
	bool isMoveableNegativeY() { return m_bMoveableNegativeY; }

	void runFrameEvent(FrameEventType_t frameEventType, int frameEventId);

	void setMoveable(bool moveable);

	void initHitRecorder(
		const SGHitSingleCallbackFn& sigleHitFn,
		const SGHitMultiCallbackFn& multiHitFn, 
		int hitListSize = 16, 
		int alreadyHitListSize = 32
	);
protected:
	// 주입 하는 필드
	HostPlayer* m_pPlayer;
	ActionInfo* m_pActionInfo;

	// 자체 필드
	HitRecorder* m_pHitRecorder;
	WeaponType_t m_eWeaponType;

	bool m_bMoveablePositiveX;
	bool m_bMoveablePositiveY;

	bool m_bMoveableNegativeX;
	bool m_bMoveableNegativeY;

	bool m_bCancelable;			
	float m_fMoveSpeedFPSX;		
	float m_fMoveSpeedFPSY;		
};
