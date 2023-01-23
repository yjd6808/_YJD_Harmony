/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 9:06:49 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>

#include "SGActionInfo.h"

struct SGActionInfo;

class SGPlayer;
class SGPlayerController;
class SGFrameTexture;
class SGActor;
class SGActorPartAnimation;
class SGAction {
public:
	SGAction(SGPlayer* player, SGActionInfo* actionInfo);
	virtual ~SGAction() = default;

	void play();
	void stop();

	virtual void init() {}	// 실행 직전 호출 (초기화 용도)
	virtual void onKeyPressed(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode);
	virtual void onKeyReleased(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode);
	virtual void onKeyPressedBefore(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode);
	virtual void onKeyReleasedBefore(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode);
	virtual void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* frame);
	virtual void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame);
	virtual void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* frame);
	virtual void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame);
	virtual void onActionBegin();
	virtual void onActionEnd(); 	// 정의안하면 해당 애니메이션 시퀀스가 끝나면 중단토록 한다.
	virtual void onUpdate(float dt) {}
	virtual bool isComboAction() { return false; }

	int getActionCode()  { return m_pActionInfo->Code; }
	int getComboKeyCount() { return m_pActionInfo->CommandCount; }
	const SGString& getActionName() { return m_pActionInfo->ActionName; }

	bool isForceCancelable() { return m_bCancelable; }
	bool isMoveable() { return m_bMoveable; }
	bool canChangeDirection() { return m_bChangeDirection; }
	void runFrameEvent(FrameEventType_t frameEventType, int frameEventId);

	float getMoveSpeedX() { return m_fMoveSpeedFPSX; }
	float getMoveSpeedY() { return m_fMoveSpeedFPSY; }
protected:
	// 주입 하는 필드
	SGPlayer* m_pPlayer;
	SGActionInfo* m_pActionInfo;

	// 자체 필드
	bool m_bMoveable;			// 액션중 움직일 수 있는지
	bool m_bCancelable;			// 액션이 캔슬될 수 있는지
	bool m_bChangeDirection;	// 액션중 방향을 바꿀 수 있는지
	float m_fMoveSpeedFPSX;		// 이때의 X방향 속도
	float m_fMoveSpeedFPSY;		// 이때의 Y방향 속도
};
