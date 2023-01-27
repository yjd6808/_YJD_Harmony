/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 9:06:49 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGActionInfo.h>

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

	// virtual void onHit();
	// virtual void onAttack();

	virtual bool isComboAction() { return false; }

	void setMoveable(bool moveable);

	int getActionCode()  { return m_pActionInfo->Code; }
	const SGString& getActionName() { return m_pActionInfo->ActionName; }
	const SGComboKeyList& getCommand() { return m_pActionInfo->Command; }
	float getMoveSpeedX() { return m_fMoveSpeedFPSX; }
	float getMoveSpeedY() { return m_fMoveSpeedFPSY; }

	bool isForceCancelable() { return m_bCancelable; }
	bool isMoveableX() { return m_bMoveableX; }
	bool isMoveableY() { return m_bMoveableY; }

	void runFrameEvent(FrameEventType_t frameEventType, int frameEventId);

	
protected:
	// 주입 하는 필드
	SGPlayer* m_pPlayer;
	SGActionInfo* m_pActionInfo;

	// 자체 필드
	bool m_bMoveableX;			
	bool m_bMoveableY;			
	bool m_bCancelable;			
	float m_fMoveSpeedFPSX;		
	float m_fMoveSpeedFPSY;		
};
