/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 10:25:33 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/Enums.h>

#include <JCore/Time.h>

class SGPlayer;
class SGCharacter;
class SGPlayerController
{
public:
	// 키와 그 키를 입력한 시간
	struct InputTime {
		JCore::DateTime Time{};
		ControlKey_t ControlKey{};
	};

	//SGPlayerController(SGPlayer* player);
public:
	//void init();
	//void update(float dt);
	//void onKeyPressed(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode);
	//void onKeyReleased(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode);
	//void onKeyPressedBefore(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode);		// 키가 눌려진 상태가 수정되기전 호출
	//void onKeyReleasedBefore(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode);	// 키가 안눌려진 상태가 수정되기전 호출
	//void onFrameBegin(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame);
	//void onFrameEnd(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame);
	//void onAnimateBegin(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame);
	//void onAnimateEnd(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame);

	//bool canRunAction();
	//void setRunningAction(ActionAbstract* action);

	//bool hasPreviousAction() { return m_pPreviousAction != nullptr; }
	//bool hasRunningAction() { return m_pRunningAction != nullptr; }
	//ActionAbstract* getRunningAction() { return m_pRunningAction; }
	//ActionAbstract* getPreviousAction() { return m_pPreviousAction; }
	//ActionAbstract* getAction(ActionType_t type);
	//ComboAction* getComboAction(const ComboKeyList& keys) { return m_ComboTree.GetComboAction(keys); }
	//void runAction(ActionType_t type);
	//void runAction(ActionAbstract* action);
	//void runComboAction(const ComboKeyList& keys);


private:
	SGPlayer* m_pPlayer;
	SGCharacter* m_pCharacter;

	// ==================================================
	// 기본 필드
	// ==================================================
	ControlKey_t m_CocosKeyCodeToControlKeyMap[200];		// 코코스 키를 ControlKey로 변환 (None(0)으로 초기화)
	int m_ControlKeyPressedMap[ControlKey::Max]{};
	InputTime m_LastestPressedKey;							// 제일 최근에 누른 키
	InputTime m_LastestReleasedKey;							// 제일 최근에 땐 키
	InputTime m_ControlKeySequence[ComboSequenceCount_v]{}; // 맨 앞이 제일 최근에 입력한 키

	bool m_bCabUseCommand{};
	SpriteDirection_t m_eDir = SpriteDirection::Right;
};