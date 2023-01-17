/*
 * 작성자: 윤정도
 * 생성일: 1/13/2023 11:42:16 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/CharacterPartAnimate.h>

class Player;
class PlayerController;
class ActionAbstract : public cocos2d::Ref {
public:
	void play();
	void stop();

	virtual bool init() = 0;
	virtual void onKeyPressed(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode);
	virtual void onKeyReleased(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode);
	virtual void onKeyPressedBefore(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode);
	virtual void onKeyReleasedBefore(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode);
	virtual void onFrameBegin(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame);
	virtual void onFrameEnd(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame);
	virtual void onAnimateBegin(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame);
	virtual void onAnimateEnd(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame);
	virtual void onActionBegin();
	virtual void onActionEnd(); 	// 정의안하면 해당 애니메이션 시퀀스가 끝나면 중단토록 한다.
	virtual void onUpdate(float dt) {}
	virtual bool isComboAction() = 0;

	int getActionType()		{ return m_iActionType;}
	bool isCancelable()		{ return m_bCancelable;		}
	bool isMoveable()		{ return m_bMoveable;		}
	float getMoveSpeedX()    { return m_fMoveSpeedFPSX;		}
	float getMoveSpeedY()    { return m_fMoveSpeedFPSY;		}
protected:

	// 주입 하는 필드
	int m_iActionType;
	Player* m_pPlayer;
	bool m_bMoveable{};			// 액션중 움직일 수 있는지
	float m_fMoveSpeedFPSX{};		// 이때의 X방향 속도
	float m_fMoveSpeedFPSY{};		// 이때의 Y방향 속도

	bool m_bCancelable{};		// 액션이 캔슬될 수 있는지

	// 기본 필드
	

	friend class PlayerActionManager;
};

